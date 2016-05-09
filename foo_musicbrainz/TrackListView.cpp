#include "foo_musicbrainz.h"
#include "TrackListView.h"
#include "Medium.h"

using namespace foo_musicbrainz;

void TrackListView::Attach(HWND window) {
	this->window = window;
}

void TrackListView::Start(t_size item, t_size subItem) {
	TableEdit_Start(item, subItem);
}

void TrackListView::Abort() {
	TableEdit_Abort(true);
}

bool TrackListView::IsActive() {
	return TableEdit_IsActive();
}

void TrackListView::set_medium(Medium *medium) {
	this->medium = medium;
}

HWND TrackListView::TableEdit_GetParentWnd() const {
	return window;
}

bool TrackListView::TableEdit_IsColumnEditable(t_size subItem) const {
	return subItem != 0;
}

void TrackListView::TableEdit_SetField(t_size item, t_size subItem, const char *value) {
	switch (subItem) {
	case 1:
		medium->get_track(item)->set_title(value);
		break;
	case 2:
		medium->get_track(item)->get_artist_credit()->set_name(value);
		break;
	}
	CTableEditHelperV2_ListView::TableEdit_SetField(item, subItem, value);
}
