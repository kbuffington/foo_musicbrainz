#pragma once

#include "foo_musicbrainz.h"
#include "TrackList.h"

namespace foo_musicbrainz {
	class Medium {
		MEMBER(int, position)
		STRING_MEMBER(title)
		POINTER_MEMBER(TrackList, track_list)

	public:
		Medium()
			: position(0),
			track_list(nullptr) {};
		~Medium() {
			POINTER_MEMBER_DESTRUCTOR(track_list)
		}
	};
}
