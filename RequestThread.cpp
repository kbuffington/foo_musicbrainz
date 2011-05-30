#include "foo_musicbrainz.h"
#include "Metadata.h"
#include "MetadataProcessor.h"
#include "Release.h"
#include "RequestThread.h"

using namespace foo_musicbrainz;

RequestThread::RequestThread(Query *query, HWND window, ReleaseList *mbc) : query(query), window(window), mbc(mbc) {}

void RequestThread::run(threaded_process_status &p_status, abort_callback &p_abort) {
	Metadata *metadata = nullptr;
	try {
		// Extracting releases from all possible locations:
		// /metadata/release
		// /metadata/release-list/release
		// /metadata/discid/release-list/release
		metadata = query->perform(p_abort);
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
				auto count = release_list->count();
				for (auto i = 0; i < count; i++) {
					release = release_list->get(i);

					// Progress and title
					p_status.set_progress(i + 1, count + 1);
					pfc::string8 title = "Fetching: ";
					title << release->get_artist_credit()->get_name() << " - " << release->get_title();
					auto date = release->get_date();
					if (date.get_year() != 0) {
						title << " (" << static_cast<pfc::string8>(date) << ")";
					}
					p_status.set_title(title);

					Query query("release", release->get_id());
					query.add_param("inc", "artists+labels+recordings+release-groups+artist-credits", false);
					auto release = query.perform(p_abort)->extract_release();
					if (release != nullptr) {
						mbc->add(release);
					}
				}
			}
		}

		if (mbc->count() == 0) {
			throw NotFound();
		}
		MetadataProcessor::apply_all(*mbc);
		mbc->sort();
		ShowWindow(window, SW_SHOW);
	} catch (GenericException e) {
		PostMessage(window, WM_CLOSE, 0, 0);
		popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
	} catch (...) {
		PostMessage(window, WM_CLOSE, 0, 0);
		delete metadata;
		delete query;
		throw;
	}

	delete metadata;
	delete query;
}
