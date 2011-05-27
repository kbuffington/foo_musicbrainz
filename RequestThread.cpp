#include "foo_musicbrainz.h"
#include "Metadata.h"
#include "Release.h"
#include "RequestThread.h"

using namespace foo_musicbrainz;

RequestThread::RequestThread(Query *query, HWND window, ReleaseList *mbc) : query(query), window(window), mbc(mbc) {}

// TODO: abort checks, progress
void RequestThread::run(threaded_process_status &p_status, abort_callback &p_abort) {
	try {
		Metadata *metadata = query->perform();
		auto release = metadata->extract_release();
		delete metadata;
		if (release == nullptr) {
			throw NotFound();
		}
		mbc->add(release);
		ShowWindow(window, SW_SHOW);
	} catch (exception_aborted) {
		PostMessage(window, WM_CLOSE, 0, 0);
	} catch (GenericException e) {
		PostMessage(window, WM_CLOSE, 0, 0);
		popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
	}
	delete query;
}
