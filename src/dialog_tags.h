#pragma once

class dialog_tags : public CDialogImpl<dialog_tags>
{
public:
	dialog_tags(const str8& p_artist, const str8& p_album) : m_artist(p_artist), m_album(p_album) {}

	BEGIN_MSG_MAP(dialog_tags)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
		COMMAND_CODE_HANDLER_EX(EN_UPDATE, OnUpdate)
	END_MSG_MAP()

	enum { IDD = IDD_SEARCH };

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		m_ok = GetDlgItem(IDOK);
		uSetDlgItemText(m_hWnd, IDC_ARTIST, m_artist);
		uSetDlgItemText(m_hWnd, IDC_ALBUM, m_album);
		CenterWindow();
		return TRUE;
	}

	void OnCloseCmd(UINT, int nID, CWindow)
	{
		uGetDlgItemText(m_hWnd, IDC_ARTIST, m_artist);
		uGetDlgItemText(m_hWnd, IDC_ALBUM, m_album);
		EndDialog(nID);
	}
	
	void OnUpdate(UINT, int, CWindow)
	{
		m_ok.EnableWindow(string_utf8_from_window(m_hWnd, IDC_ARTIST).length() && string_utf8_from_window(m_hWnd, IDC_ALBUM).length());
	}

	CButton m_ok;
	str8 m_artist;
	str8 m_album;
};
