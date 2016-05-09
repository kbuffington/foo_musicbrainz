#pragma once

#include "foo_musicbrainz.h"
#include "CoreEntity.h"
#include "ArtistCredit.h"
#include "LabelInfo.h"
#include "Medium.h"
#include "Date.h"
#include "ReleaseGroup.h"

namespace foo_musicbrainz {
	class Release : public CoreEntity {
		STRING_MEMBER(title)
		STRING_MEMBER(barcode)
		STRING_MEMBER(country)
		STRING_MEMBER(asin)
		STRING_MEMBER(language)
		STRING_MEMBER(script)
		MEMBER(int, medium_total_count)
		MEMBER_BY_REFERENCE(Date, date)
		POINTER_MEMBER(ArtistCredit, artist_credit)
		POINTER_MEMBER(ReleaseGroup, release_group)
		STRING_LIST(status, statuses, 4)
		COLLECTION(Medium, medium)
		COLLECTION(LabelInfo, label_info)

	public:
		bool is_various() const {
			auto name = get_artist_credit()->get_name();
			for (size_t i = 0; i < medium_count(); i++) {
				auto medium = get_medium(i);
				for (size_t j = 0; j < medium->track_count(); j++) {
					auto track = medium->get_track(j);
					if (track->get_artist_credit()->get_name() != name) {
						return true;
					}
				}
			}
			return false;
		}

		bool is_multidisc() const {
			return get_medium_total_count() > 1;
		}

		size_t track_count() {
			size_t tmp = 0;
			for (size_t i = 0; i < medium_count(); i++) {
				tmp+= get_medium(i)->track_count();
			}
			return tmp;
		}

		Release() :
			CoreEntity(),
			status(0),
			medium_total_count(0),
			artist_credit(nullptr),
			release_group(nullptr) {};
		~Release() {
			POINTER_MEMBER_DESTRUCTOR(artist_credit)
			POINTER_MEMBER_DESTRUCTOR(release_group)
			COLLECTION_DESTRUCTOR(medium)
			COLLECTION_DESTRUCTOR(label_info)
		}
	};
}
