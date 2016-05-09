#pragma once

#include "foo_musicbrainz.h"
#include "CoreEntity.h"

namespace foo_musicbrainz {
	class Artist : public CoreEntity {
		STRING_MEMBER(name)
		STRING_MEMBER(sort_name)

	public:
		Artist()
			: CoreEntity() {}
		~Artist() {}
	};
}
