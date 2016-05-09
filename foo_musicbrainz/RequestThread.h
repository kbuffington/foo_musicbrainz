#pragma once

#include "foo_musicbrainz.h"
#include "Query.h"
#include "ReleaseList.h"

namespace foo_musicbrainz {
	class RequestThread : public threaded_process_callback
	{
	public:
		RequestThread(Query *query, HWND window, ReleaseList *mbc, pfc::list_t<metadb_handle_ptr> *tracks);
		void run(threaded_process_status &p_status, abort_callback &p_abort);

	private:
		Query *query;
		ReleaseList *mbc;
		HWND window;
		pfc::list_t<metadb_handle_ptr> *tracks;
	};
}
