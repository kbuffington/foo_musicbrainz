#pragma once

#include "foo_musicbrainz.h"
#include "Label.h"

namespace foo_musicbrainz {
	class LabelInfo {
		STRING_MEMBER(catalog_number)
		POINTER_MEMBER(Label, label)

	public:
		LabelInfo()
			: label(nullptr) {};
		~LabelInfo() {
			POINTER_MEMBER_DESTRUCTOR(label)
		}
	};
}
