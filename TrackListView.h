#pragma once

#include "foo_musicbrainz.h"
#include "TrackList.h"

namespace foo_musicbrainz {
	class TrackListView : public InPlaceEdit::CTableEditHelperV2_ListView {
	public:
		void Attach(HWND window);
		void Start(t_size item, t_size subItem);
		void Abort();
		bool IsActive();
		void set_track_list(TrackList *track_list);

	protected:
		HWND TableEdit_GetParentWnd() const;
		bool TableEdit_IsColumnEditable(t_size subItem) const;
		void TableEdit_SetField(t_size item, t_size subItem, const char *value);

	private:
		TrackList *track_list;
		HWND window;
	};
}
