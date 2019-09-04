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
	json j = m_query->lookup(p_abort);
	if (j.size() == 0)
	{
		m_failed = true;
		return;
	}
	
	
}
