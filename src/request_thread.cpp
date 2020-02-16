#include "stdafx.h"
#include "dialog_tagger.h"
#include "request_thread.h"

namespace mb
{
	request_thread::request_thread(types type, std::unique_ptr<query> q, metadb_handle_list_cref handles)
		: m_type(type)
		, m_query(std::move(q))
		, m_handles(handles)
		, m_failed(false) {}

	void request_thread::on_done(HWND hwnd, bool was_aborted)
	{
		if (was_aborted || m_failed) return;

		if (m_release_list.size())
		{
			std::sort(m_release_list.begin(), m_release_list.end(), [](const Release& one, const Release& two) -> bool
				{
					return strcmp(one.date, two.date) < 0;
				});
			fb2k::newDialog<dialog_tagger>(m_release_list, m_handles);
		}
		else
		{
			popup_message::g_show("No matching results were found.", component_title);
		}
	}

	void request_thread::run(threaded_process_status& status, abort_callback& abort)
	{
		const size_t handle_count = m_handles.get_count();

		json j = m_query->lookup(abort);
		if (!j.is_object())
		{
			m_failed = true;
			return;
		}

		switch (m_type)
		{
		case types::discid:
			{
				str8 discid = to_str(j["id"]);
				json releases = j.value("releases", json::array());
				for (const auto& release : releases)
				{
					Release r = parser(release, handle_count, discid);
					if (r.tracks.size())
					{
						m_release_list.emplace_back(r);
					}
				}
			}
			break;
		case types::search:
			{
				json releases = j.value("releases", json::array());
				pfc::string_list_impl ids;
				filter_releases(releases, handle_count, ids);
				const size_t count = ids.get_count();

				for (size_t i = 0; i < count; ++i)
				{
					status.set_progress(i + 1, count);
					status.set_title(PFC_string_formatter() << "Fetching " << (i + 1) << " of " << count);
					Sleep(1100);

					query q("release", ids[i]);
					q.add_param("inc", "artists+labels+recordings+release-groups+artist-credits+isrcs");

					json j2 = q.lookup(abort);
					if (!j2.is_object())
					{
						m_failed = true;
						return;
					}

					Release r = parser(j2, handle_count);
					if (r.tracks.size())
					{
						m_release_list.emplace_back(r);
					}
				}
			}
			break;
		case types::albumid:
			{
				Release r = parser(j, handle_count);
				if (r.tracks.size())
				{
					m_release_list.emplace_back(r);
				}
			}
			break;
		}
	}
}
