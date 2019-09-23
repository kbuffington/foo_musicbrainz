#include "stdafx.h"
#include "dialog_tagger.h"
#include "mb_request_thread.h"

mb_request_thread::mb_request_thread(t_size p_type, mb_query* p_query, metadb_handle_list_cref p_handles) : m_type(p_type), m_query(p_query), m_handles(p_handles), m_failed(false) {}

void mb_request_thread::on_done(HWND p_wnd, bool p_was_aborted)
{
	delete m_query;
	if (p_was_aborted || m_failed) return;

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
		popup_message::g_show("No matching results were found.", COMPONENT_TITLE);
	}
}

void mb_request_thread::run(threaded_process_status& p_status, abort_callback& p_abort)
{
	t_size handle_count = m_handles.get_count();

	json j = m_query->lookup(p_abort);
	if (!j.is_object())
	{
		m_failed = true;
		return;
	}
	
	if (m_type == discid)
	{
		str8 discid = to_str(j["id"]);
		auto releases = j["releases"];
		if (releases.is_array())
		{
			for (auto& release : releases)
			{
				Release r = parser(release, handle_count);
				r.discid = discid;
				if (r.tracks.size())
				{
					m_release_list.emplace_back(r);
				}
			}
		}
	}
	else if (m_type == search)
	{
		auto releases = j["releases"];
		if (releases.is_array())
		{
			pfc::string_list_impl ids;
			filter_releases(releases, handle_count, ids);
			t_size count = ids.get_count();

			for (t_size i = 0; i < count; ++i)
			{
				p_status.set_progress(i + 1, count);
				p_status.set_title(PFC_string_formatter() << "Fetching " << (i + 1) << " of " << count);
				Sleep(1100);

				mb_query query("release", ids[i]);
				query.add_param("inc", "artists+labels+recordings+release-groups+artist-credits+isrcs");

				json j = query.lookup(p_abort);
				if (!j.is_object())
				{
					m_failed = true;
					return;
				}

				Release r = parser(j, handle_count);
				if (r.tracks.size())
				{
					m_release_list.emplace_back(r);
				}
			}
		}
	}
	else if (m_type == albumid)
	{
		Release r = parser(j, handle_count);
		if (r.tracks.size())
		{
			m_release_list.emplace_back(r);
		}
	}
}
