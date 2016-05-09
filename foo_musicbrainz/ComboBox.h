#pragma once

#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	class ComboBox : public CComboBox {
	public:
		void DeleteAll() {
			auto count = GetCount();
			for (size_t i = 0; i < count; i++) {
				DeleteString(0);
			}
		}

		void SetCurrentString(const wchar_t *str) {
			auto index = GetCurSel();
			DeleteString(index);
			this->InsertString(index, str);
			SetCurSel(index);
		}
	};
}
