#pragma once

#include "foo_musicbrainz.h"
#include "CoreEntity.h"

namespace foo_musicbrainz {
	class ReleaseGroup : public CoreEntity {
		STRING_MEMBER(title)
		STRING_LIST(type, types, 12)

	public:
		ReleaseGroup() :
			CoreEntity(),
			type(0) {};
		~ReleaseGroup() {}
	};
}
