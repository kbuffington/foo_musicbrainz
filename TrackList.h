#pragma once

#include "foo_musicbrainz.h"
#include "EntityList.h"
#include "Track.h"

namespace foo_musicbrainz {
	class TrackList : public EntityList<Track> {
	};
}
