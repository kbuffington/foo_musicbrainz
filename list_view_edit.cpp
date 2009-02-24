#include "foo_musicbrainz.h"

HWND list_view_edit::TableEdit_GetParentWnd() const
{
	return window;
}

void list_view_edit::start(t_size item, t_size subItem)
{
	TableEdit_Start(item, subItem);
}

list_view_edit::list_view_edit(HWND _window)
{
	window = _window;
}

bool list_view_edit::TableEdit_IsColumnEditable(t_size subItem) const
{
	if (subItem == 0 && TableEdit_GetColumnCount() == 2) return false;
	else return true;
}

void list_view_edit::TableEdit_Finished()
{
}
