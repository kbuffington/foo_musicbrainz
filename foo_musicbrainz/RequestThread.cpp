#include "foo_musicbrainz.h"
#include "Metadata.h"
#include "MetadataProcessor.h"
#include "Release.h"
#include "RequestThread.h"

using namespace foo_musicbrainz;

RequestThread::RequestThread(Query *query, HWND window, ReleaseList *mbc, pfc::list_t<metadb_handle_ptr> *tracks)
	: query(query),
	window(window),
	mbc(mbc),
	tracks(tracks) {}

void RequestThread::run(threaded_process_status &p_status, abort_callback &p_abort) {
	Metadata *metadata = nullptr;
	try {
		metadata = query->perform(p_abort);
		
		// Extracting releases from 3 possible locations
		// /metadata/release
		if (auto release = metadata->extract_release()) {
			mbc->add(release);

		// /metadata/discid/release-list/release
		} else if (auto discid = metadata->get_discid()) {
			if (auto release_list = discid->get_release_list()) {
				auto id = discid->get_id();
				auto count = release_list->count();
				for (size_t i = 0; i < count; i++) {
					auto release = release_list->extract(0);
					// FIXME: this is incorrect, as only one of the discs has
					// this discid, but it won't affect anyone (release must
					// have two or more discs with the same amount of tracks,
					// and user must choose the wrong one). Will be fixed when
					// web service adds support for inc=discids param.
					for (size_t i = 0; i < release->medium_count(); i++) {
						release->get_medium(i)->set_discid(id);
					}
					mbc->add(release);
				}
			}

		// /metadata/release-list/release
		} else if (auto release_list = metadata->get_release_list()) {
			auto count = release_list->count();
			for (size_t i = 0; i < count; i++) {
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

		// Removing discs from multidisc releases with different track count
		auto track_count = tracks->get_count();
		for (size_t i = 0; i < mbc->count(); i++) {
			auto release = mbc->get(i);
			if (release->track_count() == track_count) continue;
			for (size_t j = 0; j < release->medium_count(); j++) {
				if (release->get_medium(j)->track_count() != track_count) {
					release->remove_medium(j);
					j--;
				}
			}
			if (release->medium_count() == 0) {
				mbc->remove(i);
				i--;
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
