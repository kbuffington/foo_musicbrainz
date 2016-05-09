#pragma once

#include "foo_musicbrainz.h"
#include "Recording.h"

namespace foo_musicbrainz {
	class Track : public Recording {
		MEMBER(int, position)

	public:
		Track()
			: Recording(),
			position(0) {}
		~Track() {}
	};
}
