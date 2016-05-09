#pragma once

#include "foo_musicbrainz.h"
#include "ReleaseList.h"

namespace foo_musicbrainz {
	class DiscID {
		STRING_MEMBER(id)
		MEMBER(int, sectors)
		POINTER_MEMBER(ReleaseList, release_list)

	public:
		DiscID()
			: sectors(0),
			release_list(nullptr) {}
		~DiscID() {
			POINTER_MEMBER_DESTRUCTOR(release_list)
		}
	};
}
