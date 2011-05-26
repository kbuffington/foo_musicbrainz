#pragma once

#include "foo_musicbrainz.h"
#include "CoreEntity.h"

namespace foo_musicbrainz {
	class ArtistCredit : public CoreEntity {
		STRING_MEMBER(name)

	public:
		ArtistCredit() {};
	};
}
