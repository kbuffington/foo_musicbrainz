#pragma once

#include "foo_musicbrainz.h"
#include "ArtistCredit.h"
#include "Recording.h"

namespace foo_musicbrainz {
	class Track {
		MEMBER(int, position)
		POINTER_MEMBER(Recording, recording)

	public:
		Track()
			: recording(nullptr) {};
		~Track() {
			POINTER_MEMBER_DESTRUCTOR(recording)
		}
	};
}
