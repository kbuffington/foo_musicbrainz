#pragma once

#include "foo_musicbrainz.h"
#include "EntityList.h"
#include "LabelInfo.h"

namespace foo_musicbrainz {
	class LabelInfoList : public EntityList<LabelInfo> {
	};
}
