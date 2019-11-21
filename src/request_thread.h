#pragma once
#include "query.h"

namespace mb
{
	class request_thread : public threaded_process_callback
	{
	public:
		enum class types
		{
			discid,
			search,
			albumid
		};

		request_thread(types p_type, query* p_query, metadb_handle_list_cref p_handles);

		void on_done(HWND p_wnd, bool p_was_aborted) override;
		void run(threaded_process_status& p_status, abort_callback& p_abort) override;

	private:
		bool m_failed;
		metadb_handle_list m_handles;
		query* m_query;
		std::vector<Release> m_release_list;
		types m_type;
	};
}
