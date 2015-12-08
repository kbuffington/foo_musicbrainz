#pragma once

#include "foo_musicbrainz.h"
#include "Label.h"

namespace foo_musicbrainz {
	class LabelInfo : public Label {
		STRING_MEMBER(catalog_number)

	public:
		pfc::string8 get_info() {
			auto label = get_name();
			auto cat = get_catalog_number();
			auto label_empty = label.is_empty();
			auto cat_empty = cat.is_empty();
			if (label_empty && cat_empty) {
				return "-";
			}
			pfc::string8 labelcat;
			labelcat << (label_empty ? "?" : label);
			labelcat << "/";
			labelcat << (cat_empty ? "?" : cat);
			return labelcat;
		}

		LabelInfo() {};
		~LabelInfo() {}
	};
}
