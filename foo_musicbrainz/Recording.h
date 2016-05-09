#pragma once

#include "foo_musicbrainz.h"
#include "ArtistCredit.h"
#include "CoreEntity.h"

namespace foo_musicbrainz {
	class Recording : public CoreEntity {
		MEMBER(int, length)
		STRING_MEMBER(title)
		POINTER_MEMBER(ArtistCredit, artist_credit)

	public:
		Recording()
			: CoreEntity(),
			length(0),
			artist_credit(nullptr) {}
		~Recording() {
			POINTER_MEMBER_DESTRUCTOR(artist_credit)
		}
	};
}
