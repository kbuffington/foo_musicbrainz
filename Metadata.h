#pragma once

#include "foo_musicbrainz.h"
#include "Release.h"
#include "ReleaseList.h"

namespace foo_musicbrainz {
	class Metadata {
		POINTER_MEMBER(Release, release)
		POINTER_MEMBER(ReleaseList, release_list)

	public:
		Metadata()
			: release(nullptr),
			release_list(nullptr) {}
		~Metadata() {
			POINTER_MEMBER_DESTRUCTOR(release)
			POINTER_MEMBER_DESTRUCTOR(release_list)
		}
	};
}
