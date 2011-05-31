#pragma once

#include "foo_musicbrainz.h"
#include "Track.h"

namespace foo_musicbrainz {
	class Medium {
		MEMBER(int, position)
		STRING_MEMBER(title)
		COLLECTION(Track, track)

	public:
		Medium()
			: position(0) {};
		~Medium() {
			COLLECTION_DESTRUCTOR(track)
		}
	};
}
