#include "stdafx.h"
#include "mb_request_thread.h"

mb_request_thread::mb_request_thread(mb_query* p_query, metadb_handle_list_cref p_handles) : m_query(p_query), m_handles(p_handles), m_failed(false) {}

void mb_request_thread::on_done(HWND p_wnd, bool p_was_aborted)
{
	if (p_was_aborted || m_failed) return;

	//spawn tagger dialog here
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
	
	if (j.find("sectors") != j.end()) //discid
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
}
