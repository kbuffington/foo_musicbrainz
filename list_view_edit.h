#pragma once

class track_list_view_edit : public InPlaceEdit::CTableEditHelperV2_ListView {
public:
	track_list_view_edit(HWND _window, mbCollection *_mbc);
	void start(t_size item, t_size subItem);
	HWND TableEdit_GetParentWnd() const;
	bool TableEdit_Advance(t_size & item, t_size & subItem, t_uint32 whathappened);
	mbCollection *mbc;
	HWND window;

protected:
	bool TableEdit_IsColumnEditable(t_size subItem) const;
};
