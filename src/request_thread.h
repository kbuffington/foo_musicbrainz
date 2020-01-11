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

		request_thread(types type, query* q, metadb_handle_list_cref handles);

		void on_done(HWND hwnd, bool was_aborted) override;
		void run(threaded_process_status& status, abort_callback& abort) override;

	private:
		bool m_failed;
		metadb_handle_list m_handles;
		query* m_query;
		std::vector<Release> m_release_list;
		types m_type;
	};
}
