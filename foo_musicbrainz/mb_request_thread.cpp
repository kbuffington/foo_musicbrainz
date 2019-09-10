#include "stdafx.h"
#include "dialog_tagger.h"
#include "mb_request_thread.h"

mb_request_thread::mb_request_thread(mb_query* p_query, metadb_handle_list_cref p_handles) : m_query(p_query), m_handles(p_handles), m_failed(false) {}

void mb_request_thread::on_done(HWND p_wnd, bool p_was_aborted)
{
	if (p_was_aborted || m_failed) return;

	if (m_release_list.get_count() > 0)
	{
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
	if (j.size() == 0)
	{
		m_failed = true;
		return;
	}
	
	if (j.find("sectors") != j.end()) // discid
	{
		str8 discid = to_str(j["id"]);
		auto releases = j["releases"];
		if (releases.is_array())
		{
			for (auto& release : releases)
			{
				Release r = parser(release, handle_count);
				if (r.discs.get_count() > 0)
				{
					r.discid = discid;
					m_release_list.add_item(r);
				}
			}
		}
	}
	else if (j.find("releases") != j.end()) // artist/album lookup
	{
		auto releases = j["releases"];
		if (releases.is_array())
		{
			t_size count = releases.size();
			for (t_size i = 0; i < count; ++i)
			{
				p_status.set_progress(i + 1, count);
				p_status.set_title(PFC_string_formatter() << "Fetching " << (i + 1) << " of " << count);
				Sleep(1100);

				auto query = new mb_query("release", to_str(releases[i]["id"]));
				query->add_param("inc", "artists+labels+recordings+release-groups+artist-credits");

				json j = query->lookup(p_abort);
				if (j.size() == 0)
				{
					m_failed = true;
					return;
				}

				Release r = parser(j, handle_count);
				if (r.discs.get_count() > 0)
				{
					m_release_list.add_item(r);
				}
			}
		}
	}
	else // mbid lookup
	{
		Release r = parser(j, handle_count);
		if (r.discs.get_count() > 0)
		{
			m_release_list.add_item(r);
		}
	}
}
