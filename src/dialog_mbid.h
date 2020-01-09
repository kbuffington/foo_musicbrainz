#pragma once

namespace mb
{
	class dialog_mbid : public CDialogImpl<dialog_mbid>
	{
	public:
		dialog_mbid(const str8& p_albumid_str) : m_albumid_str(p_albumid_str) {}

		BEGIN_MSG_MAP_EX(dialog_mbid)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
			COMMAND_CODE_HANDLER_EX(EN_UPDATE, OnUpdate)
		END_MSG_MAP()

		enum { IDD = IDD_ALBUMID };

		BOOL OnInitDialog(CWindow, LPARAM)
		{
			m_ok = GetDlgItem(IDOK);
			m_albumid_edit = GetDlgItem(IDC_EDIT_MBID);

			uSetWindowText(m_albumid_edit, m_albumid_str);
			CenterWindow();
			return TRUE;
		}

		void OnCloseCmd(UINT, int nID, CWindow)
		{
			uGetWindowText(m_albumid_edit, m_albumid_str);
			EndDialog(nID);
		}

		void OnUpdate(UINT, int, CWindow)
		{
			str8 t;
			uGetWindowText(m_albumid_edit, t);
			str8 u = prefs::get_server();
			u << "/release/";
			const size_t l = u.get_length();
			if (strncmp(t, u, l) == 0)
			{
				t.replace_string(u, "");
				uSetWindowText(m_albumid_edit, t);
				return;
			}

			m_ok.EnableWindow(is_uuid(t.get_ptr()));
		}

		CButton m_ok;
		CEdit m_albumid_edit;
		str8 m_albumid_str;
	};
}
