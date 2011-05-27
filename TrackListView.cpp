#include "foo_musicbrainz.h"
#include "TrackListView.h"
#include "ReleaseList.h"

using namespace foo_musicbrainz;

void TrackListView::Attach(HWND window, ReleaseList *mbc) {
	this->window = window;
	this->mbc = mbc;
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

HWND TrackListView::TableEdit_GetParentWnd() const {
	return window;
}

bool TrackListView::TableEdit_IsColumnEditable(t_size subItem) const {
	return subItem != 0;
}

void TrackListView::TableEdit_SetField(t_size item, t_size subItem, const char *value) {
	//switch (subItem) {
	//case 1:
	//	mbc->getRelease()->getTrack(item)->setTitle(value);
	//	break;
	//case 2:
	//	mbc->getRelease()->getTrack(item)->setArtist(value);
	//	break;
	//}
	CTableEditHelperV2_ListView::TableEdit_SetField(item, subItem, value);
}
