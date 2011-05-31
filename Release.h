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
		MEMBER_BY_REFERENCE(Date, date)
		POINTER_MEMBER(ArtistCredit, artist_credit)
		POINTER_MEMBER(ReleaseGroup, release_group)
		STRING_LIST(status, statuses, 4)
		COLLECTION(Medium, medium)
		COLLECTION(LabelInfo, label_info)

	public:
		bool is_various() {
			return false;
		}

		Release() :
			CoreEntity(),
			status(0),
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
