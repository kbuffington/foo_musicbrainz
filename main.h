#ifndef __MAIN_H__
#define __MAIN_H__

class list_view_edit : public InPlaceEdit::CTableEditHelperV2_ListView {
public:
	HWND TableEdit_GetParentWnd() const;
	list_view_edit(HWND _window);
	void start(t_size item, t_size subItem);
	void TableEdit_Finished();

protected:
	bool TableEdit_IsColumnEditable(t_size subItem) const;

private:
	HWND window;
};

BOOL CALLBACK TaggerDialogProc(HWND tagger_dialog, UINT Message, WPARAM wParam, LPARAM lParam);

#endif
