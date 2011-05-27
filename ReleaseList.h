#pragma once

#include "foo_musicbrainz.h"
#include "EntityList.h"
#include "Release.h"

namespace foo_musicbrainz {
	class ReleaseList : public EntityList<Release> {
	};
}
