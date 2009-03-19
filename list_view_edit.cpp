#include "foo_musicbrainz.h"

track_list_view_edit::track_list_view_edit(HWND _window, mbCollection *_mbc)
{
	window = _window;
	mbc = _mbc;
}

void track_list_view_edit::start(t_size item, t_size subItem)
{
	TableEdit_Start(item, subItem);
}

HWND track_list_view_edit::TableEdit_GetParentWnd() const
{
	return window;
}

bool track_list_view_edit::TableEdit_Advance(t_size & item, t_size & subItem, t_uint32 whathappened)
{
	if (whathappened & 0x100)
	{
		pfc::string8 str;
		t_size tmp;
		switch (subItem)
		{
		case 1:
			TableEdit_GetField(item, subItem, str, tmp);
			mbc->getRelease()->getTrack(item)->setTitle(str);
			break;
		case 2:
			TableEdit_GetField(item, subItem, str, tmp);
			mbc->getRelease()->getTrack(item)->setArtist(str);
			break;
		}
	}
	return CTableEditHelperV2::TableEdit_Advance(item, subItem, whathappened);
}

bool track_list_view_edit::TableEdit_IsColumnEditable(t_size subItem) const
{
	if (subItem == 0) return false;
	else return true;
}
