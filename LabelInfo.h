#pragma once

#include "foo_musicbrainz.h"
#include "Label.h"

namespace foo_musicbrainz {
	class LabelInfo : public Label {
		STRING_MEMBER(catalog_number)

	public:
		LabelInfo() {};
		~LabelInfo() {}
	};
}
