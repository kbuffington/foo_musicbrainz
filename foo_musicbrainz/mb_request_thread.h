#pragma once
#include "mb_query.h"

class mb_request_thread : public threaded_process_callback
{
public:
	mb_request_thread(mb_query* query, metadb_handle_list_cref p_handles);

	void on_done(HWND p_wnd, bool p_was_aborted) override;
	void run(threaded_process_status& p_status, abort_callback& p_abort) override;

private:
	bool m_failed;
	mb_query* m_query;
	metadb_handle_list m_handles;
};
