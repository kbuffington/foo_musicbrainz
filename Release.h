#pragma once

#include "foo_musicbrainz.h"
#include "CoreEntity.h"
#include "ArtistCredit.h"
#include "MediumList.h"
#include "Date.h"
#include "ReleaseGroup.h"

namespace foo_musicbrainz {
	class Release : public CoreEntity {
		STRING_MEMBER(title)
		STRING_MEMBER(barcode)
		STRING_MEMBER(country)
		STRING_MEMBER(asin)
		MEMBER_BY_REFERENCE(Date, date)
		POINTER_MEMBER(ArtistCredit, artist_credit)
		POINTER_MEMBER(MediumList, medium_list)
		POINTER_MEMBER(ReleaseGroup, release_group)
		STRING_LIST(status, statuses, 4)

	public:
		Release() :
			CoreEntity(),
			artist_credit(nullptr),
			medium_list(nullptr) {};
		~Release() {
			POINTER_MEMBER_DESTRUCTOR(artist_credit)
			POINTER_MEMBER_DESTRUCTOR(medium_list)
		}
	};
}
