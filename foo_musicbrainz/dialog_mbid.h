#pragma once

class dialog_mbid : public CDialogImpl<dialog_mbid>
{
public:
	dialog_mbid(const str8& p_album_id) : m_album_id(p_album_id) {}

	BEGIN_MSG_MAP(dialog_mbid)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
		COMMAND_CODE_HANDLER_EX(EN_UPDATE, OnUpdate)
	END_MSG_MAP()

	enum { IDD = IDD_CUSTOM_QUERY_MBID };

	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam)
	{
		m_ok = GetDlgItem(IDOK);
		uSetDlgItemText(m_hWnd, IDC_MBID, m_album_id);
		CenterWindow();
		return true;
	}

	void OnCloseCmd(UINT uNotifyCode, int nID, HWND wndCtl)
	{
		uGetDlgItemText(m_hWnd, IDC_MBID, m_album_id);
		EndDialog(nID);
	}
	
	void OnUpdate(UINT uNotifyCode, int nID, HWND wndCtl)
	{
		str8 t = string_utf8_from_window(m_hWnd, IDC_MBID).get_ptr();
		str8 u = "https://musicbrainz.org/release/";
		t_size l = u.get_length();
		if (strncmp(t, u, l) == 0)
		{
			t.replace_string(u, "");
			uSetDlgItemText(m_hWnd, IDC_MBID, t);
			return;
		}

		std::regex rx("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$");
		m_ok.EnableWindow(regex_search(string_utf8_from_window(m_hWnd, IDC_MBID).get_ptr(), rx));
	}

	CButton m_ok;
	str8 m_album_id;
};
