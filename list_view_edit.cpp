#include "foo_musicbrainz.h"
#include "list_view_edit.h"
#include "ReleaseList.h"

using namespace foo_musicbrainz;

void track_list_view_edit::Attach(HWND window, ReleaseList *mbc) {
	this->window = window;
	this->mbc = mbc;
}

void track_list_view_edit::Start(t_size item, t_size subItem) {
	TableEdit_Start(item, subItem);
}

void track_list_view_edit::Abort() {
	TableEdit_Abort(true);
}

bool track_list_view_edit::IsActive() {
	return TableEdit_IsActive();
}

HWND track_list_view_edit::TableEdit_GetParentWnd() const {
	return window;
}

bool track_list_view_edit::TableEdit_IsColumnEditable(t_size subItem) const {
	return subItem != 0;
}

void track_list_view_edit::TableEdit_SetField(t_size item, t_size subItem, const char *value) {
	switch (subItem) {
	//case 1:
	//	mbc->getRelease()->getTrack(item)->setTitle(value);
	//	break;
	//case 2:
	//	mbc->getRelease()->getTrack(item)->setArtist(value);
	//	break;
	}
	CTableEditHelperV2_ListView::TableEdit_SetField(item, subItem, value);
}
