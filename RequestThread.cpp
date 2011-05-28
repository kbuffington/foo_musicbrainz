#include "foo_musicbrainz.h"
#include "Metadata.h"
#include "Release.h"
#include "RequestThread.h"

using namespace foo_musicbrainz;

RequestThread::RequestThread(Query *query, HWND window, ReleaseList *mbc) : query(query), window(window), mbc(mbc) {}

// TODO: abort checks, progress
void RequestThread::run(threaded_process_status &p_status, abort_callback &p_abort) {
	try {
		// Extracting releases from all possible locations:
		// /metadata/release
		// /metadata/release-list/release
		// /metadata/discid/release-list/release
		auto metadata = query->perform();
		auto release = metadata->extract_release();
		if (release != nullptr) {
			mbc->add(release);
		} else {
			auto release_list = metadata->get_release_list();
			if (release_list == nullptr) {
				auto discid = metadata->get_discid();
				if (discid != nullptr) {
					release_list = discid->get_release_list();
				}
			}
			if (release_list != nullptr) {
				while (release_list->count() > 0) {
					mbc->add(release_list->extract(0));
				}
			}
		}

		delete metadata;
		if (mbc->count() == 0) {
			throw NotFound();
		}
		ShowWindow(window, SW_SHOW);
	} catch (exception_aborted) {
		PostMessage(window, WM_CLOSE, 0, 0);
	} catch (GenericException e) {
		PostMessage(window, WM_CLOSE, 0, 0);
		popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
	}
	delete query;
}
