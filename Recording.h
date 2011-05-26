#pragma once

#include "foo_musicbrainz.h"
#include "CoreEntity.h"

namespace foo_musicbrainz {
	class Recording : public CoreEntity {
		MEMBER(int, length)
		STRING_MEMBER(title)

	public:
		Recording() :
			CoreEntity() {};
		~Recording() {}
	};
}
