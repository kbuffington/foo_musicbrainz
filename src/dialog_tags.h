#pragma once

namespace mb
{
	class dialog_tags : public CDialogImpl<dialog_tags>
	{
	public:
		dialog_tags(const str8& p_artist_str, const str8& p_album_str) : m_artist_str(p_artist_str), m_album_str(p_album_str) {}

		BEGIN_MSG_MAP(dialog_tags)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
			COMMAND_CODE_HANDLER_EX(EN_UPDATE, OnUpdate)
		END_MSG_MAP()

		enum { IDD = IDD_SEARCH };

		BOOL OnInitDialog(CWindow, LPARAM)
		{
			m_ok = GetDlgItem(IDOK);
			m_artist_edit = GetDlgItem(IDC_EDIT_ARTIST);
			m_album_edit = GetDlgItem(IDC_EDIT_ALBUM);

			uSetWindowText(m_artist_edit, m_artist_str);
			uSetWindowText(m_album_edit, m_album_str);
			CenterWindow();
			return TRUE;
		}

		void OnCloseCmd(UINT, int nID, CWindow)
		{
			uGetWindowText(m_artist_edit, m_artist_str);
			uGetWindowText(m_album_edit, m_album_str);
			EndDialog(nID);
		}

		void OnUpdate(UINT, int, CWindow)
		{
			str8 ar, al;
			uGetWindowText(m_artist_edit, ar);
			uGetWindowText(m_album_edit, al);
			m_ok.EnableWindow(ar.get_length() && al.get_length());
		}

		CButton m_ok;
		CEdit m_artist_edit;
		CEdit m_album_edit;
		str8 m_artist_str;
		str8 m_album_str;
	};
}
