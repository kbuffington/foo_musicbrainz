#pragma once

#include <regex>
#include "Query.h"
#include "TaggerDialog.h"

namespace foo_musicbrainz {
	class QueryByMBIDDialog : public CDialogImpl<QueryByMBIDDialog> {
	private:
		pfc::list_t<metadb_handle_ptr> tracks;
		CButton ok;
		pfc::string8 mbid;

	public:
		enum { IDD = IDD_CUSTOM_QUERY_MBID };

		QueryByMBIDDialog(pfc::list_t<metadb_handle_ptr> _tracks, pfc::string8 &_mbid)
			: CDialogImpl<QueryByMBIDDialog>(),
			tracks(_tracks),
			mbid(_mbid)
		{
			Create(core_api::get_main_window());
		}

		BEGIN_MSG_MAP(CCustomQueryMBID)
			MSG_WM_INITDIALOG(OnInitDialog)
			MSG_WM_CLOSE(OnClose)
			COMMAND_HANDLER_EX(IDC_MBID, EN_UPDATE, OnUpdate)
			COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
			COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		END_MSG_MAP()

		BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {
			static_api_ptr_t<modeless_dialog_manager>()->add(m_hWnd);
			ok = GetDlgItem(IDOK);
			uSetDlgItemText(m_hWnd, IDC_MBID, mbid);
			return true;
		}

		void OnClose() {
			DestroyWindow();
		}

		void OnFinalMessage(HWND hwnd) {
			static_api_ptr_t<modeless_dialog_manager>()->remove(m_hWnd);
			delete this;
		}
	
		void OnUpdate(UINT uNotifyCode, int nID, CWindow wndCtl) {
			std::tr1::regex rx("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$");
			if (regex_search(string_utf8_from_window(m_hWnd, IDC_MBID).get_ptr(), rx)) {
				ok.EnableWindow(true);
			}
			else ok.EnableWindow(false);
		}

		void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl) {
			DestroyWindow();
		}

		void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl) {
			pfc::string8 id = string_utf8_from_window(m_hWnd, IDC_MBID);
			auto query = new Query("release", id);
			query->add_param("inc", "artists+labels+recordings+release-groups+artist-credits", false);
			new TaggerDialog(query, tracks);
			DestroyWindow();
		}
	};
}
