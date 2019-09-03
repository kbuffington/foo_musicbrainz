#pragma once
#include "Artist.h"

namespace foo_musicbrainz {
	class NameCredit {
		STRING_MEMBER(name)
		STRING_MEMBER(joinphrase)
		POINTER_MEMBER(Artist, artist)

	public:
		NameCredit()
			: artist(nullptr) {}
		~NameCredit() {
			POINTER_MEMBER_DESTRUCTOR(artist)
		}
	};
}
