#pragma once

namespace mb
{
	class dialog_tags : public CDialogImpl<dialog_tags>
	{
	public:
		dialog_tags(pfc::stringp artist_str, pfc::stringp album_str, pfc::stringp album_stripped_str) : m_artist_str(artist_str), m_album_str(album_str), m_album_stripped_str(album_stripped_str) {}

		BEGIN_MSG_MAP_EX(dialog_tags)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
			COMMAND_CODE_HANDLER_EX(EN_UPDATE, OnUpdate)
			COMMAND_RANGE_HANDLER_EX(IDC_CHECK_ALBUM_EDITION, IDC_CHECK_ALBUM_EDITION, OnToggle)
		END_MSG_MAP()

		enum { IDD = IDD_SEARCH };

		BOOL OnInitDialog(CWindow, LPARAM)
		{
			m_ok = GetDlgItem(IDOK);
			m_artist_edit = GetDlgItem(IDC_EDIT_ARTIST);
			m_album_edit = GetDlgItem(IDC_EDIT_ALBUM);
			m_use_edition_checkbox = GetDlgItem(IDC_CHECK_ALBUM_EDITION);

			uSetWindowText(m_artist_edit, m_artist_str);
			uSetWindowText(m_album_edit, m_album_stripped_str);
			m_use_edition_checkbox.SetCheck(true);
			if (!strcmp(m_album_str, m_album_stripped_str)) {
				m_use_edition_checkbox.ShowWindow(SW_HIDE);
			}
			CenterWindow();
			return TRUE;
		}

		void OnCloseCmd(UINT, int nID, CWindow)
		{
			uGetWindowText(m_artist_edit, m_artist_str);
			uGetWindowText(m_album_edit, m_album_str);
			EndDialog(nID);
		}

		void OnToggle(UINT a, int b, CWindow wnd)
		{
			bool checked = m_use_edition_checkbox.IsChecked();
			if (checked != lastCheckVal) {
				lastCheckVal = checked;
				if (checked) {
					uSetWindowText(m_album_edit, m_album_stripped_str);
				}
				else {
					uSetWindowText(m_album_edit, m_album_str);
				}
			}
			OnUpdate(a, b, wnd);
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
		CCheckBox m_use_edition_checkbox;
		str8 m_artist_str;
		str8 m_album_str;
		str8 m_album_stripped_str;
		bool lastCheckVal = true;
	};
}
