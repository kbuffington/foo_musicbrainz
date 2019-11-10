#pragma once
#include "query.h"

class mb_request_thread : public threaded_process_callback
{
public:
	mb_request_thread(t_size p_type, mb_query* p_query, metadb_handle_list_cref p_handles);

	void on_done(HWND p_wnd, bool p_was_aborted) override;
	void run(threaded_process_status& p_status, abort_callback& p_abort) override;

	enum
	{
		discid,
		search,
		albumid
	};

private:
	bool m_failed;
	mb_query* m_query;
	metadb_handle_list m_handles;
	std::vector<Release> m_release_list;
	t_size m_type;
};
