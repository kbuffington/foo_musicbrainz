#include "foo_musicbrainz.h"
#include "Release.h"
#include "RequestThread.h"

using namespace foo_musicbrainz;

RequestThread::RequestThread(Query *query, HWND window, ReleaseList *mbc) : query(query), window(window), mbc(mbc) {}

void RequestThread::run(threaded_process_status &p_status, abort_callback &p_abort) {
	try {
		Release *release = query->get_release();
		mbc->add(release);
		PostMessage(window, WM_FOO_MB_UPDATE_RELEASES_LIST, 0, 0);
		ShowWindow(window, SW_SHOW);
	} catch (exception_foo_mb_connection_error e) {
		PostMessage(window, WM_CLOSE, 0, 0);
		popup_message::g_show(e.g_what(), COMPONENT_TITLE, popup_message::icon_error);
	} catch (exception_aborted) {
		PostMessage(window, WM_CLOSE, 0, 0);
	} catch (exception_foo_mb_xml_parsing e) {
		PostMessage(window, WM_CLOSE, 0, 0);
		pfc::string8 error_sting = e.what();
		popup_message::g_show(error_sting, COMPONENT_TITLE, popup_message::icon_error);
	} catch (exception_foo_mb_no_releases e) {
		PostMessage(window, WM_CLOSE, 0, 0);
		popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
	} catch (ticpp::Exception e) {
		PostMessage(window, WM_CLOSE, 0, 0);
		popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
	}
	delete query;
}
