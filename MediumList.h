#pragma once

#include "foo_musicbrainz.h"
#include "EntityList.h"
#include "Medium.h"

namespace foo_musicbrainz {
	class MediumList : public EntityList<Medium> {
	};
}
