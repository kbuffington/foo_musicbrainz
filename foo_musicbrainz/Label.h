#pragma once

#include "foo_musicbrainz.h"
#include "CoreEntity.h"

namespace foo_musicbrainz {
	class Label : public CoreEntity {
		STRING_MEMBER(name)
		STRING_MEMBER(sort_name)
		STRING_MEMBER(label_code)

	public:
		Label() {};
		~Label() {}
	};
}
