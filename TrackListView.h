#pragma once

#include "foo_musicbrainz.h"
#include "ReleaseList.h"

namespace foo_musicbrainz {
	class TrackListView : public InPlaceEdit::CTableEditHelperV2_ListView {
	public:
		void Attach(HWND window, ReleaseList *mbc);
		void Start(t_size item, t_size subItem);
		void Abort();
		bool IsActive();

	protected:
		HWND TableEdit_GetParentWnd() const;
		bool TableEdit_IsColumnEditable(t_size subItem) const;
		void TableEdit_SetField(t_size item, t_size subItem, const char *value);

	private:
		ReleaseList *mbc;
		HWND window;
	};
}
