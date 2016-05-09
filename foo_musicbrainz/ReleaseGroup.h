#pragma once

#include "foo_musicbrainz.h"
#include "CoreEntity.h"
#include "Date.h"

namespace foo_musicbrainz {
	class ReleaseGroup : public CoreEntity {
		STRING_MEMBER(title)
		STRING_LIST(type, types, 12)
		MEMBER_BY_REFERENCE(Date, first_release_date)

	public:
		ReleaseGroup() :
			CoreEntity(),
			type(0) {};
		~ReleaseGroup() {}
	};
}
