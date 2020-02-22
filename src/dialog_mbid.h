#pragma once

namespace mb
{
	class dialog_mbid : public CDialogImpl<dialog_mbid>
	{
	public:
		dialog_mbid(pfc::stringp albumid_str, str8 group_title_str) 
			: m_mbid_str(albumid_str)
			, m_group_title_str(group_title_str) {}

		BEGIN_MSG_MAP_EX(dialog_mbid)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
			COMMAND_CODE_HANDLER_EX(EN_UPDATE, OnUpdate)
		END_MSG_MAP()

		enum { IDD = IDD_ALBUMID };

		BOOL OnInitDialog(CWindow, LPARAM)
		{
			m_ok = GetDlgItem(IDOK);
			m_mbid_edit = GetDlgItem(IDC_EDIT_MBID);
			CEdit m_mbid_title = GetDlgItem(IDC_STATIC);

			uSetWindowText(m_mbid_edit, m_mbid_str);
			uSetWindowText(m_mbid_title, m_group_title_str);
			CenterWindow();
			return TRUE;
		}

		void OnCloseCmd(UINT, int nID, CWindow)
		{
			uGetWindowText(m_mbid_edit, m_mbid_str);
			EndDialog(nID);
		}

		void OnUpdate(UINT, int, CWindow)
		{
			str8 t;
			uGetWindowText(m_mbid_edit, t);
			str8 u = prefs::get_server();
			u << "/release/";
			const size_t l = u.get_length();
			if (strncmp(t, u, l) == 0)
			{
				t.replace_string(u, "");
				uSetWindowText(m_mbid_edit, t);
				return;
			}

			m_ok.EnableWindow(is_uuid(t.get_ptr()));
		}

		CButton m_ok;
		CEdit m_mbid_edit;
		str8 m_mbid_str;
		str8 m_group_title_str;
	};
}
