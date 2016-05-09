#pragma once

#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	class ListView : public CListViewCtrl {
	public:
		ListView& operator =(HWND hWnd) {
			m_hWnd = hWnd;
			return *this;
		}

		void Resize(size_t new_size) {
			auto current_size = GetItemCount();
			for (; current_size > new_size; current_size--) {
				DeleteItem(current_size - 1);
			}
			for (; current_size < new_size; current_size++) {
				InsertItem(current_size, L"");
			}
		}
	};
}
