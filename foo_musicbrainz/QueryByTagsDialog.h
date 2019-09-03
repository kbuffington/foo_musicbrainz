#pragma once
#include "ReleaseList.h"
#include "TaggerDialog.h"

namespace foo_musicbrainz {
	class QueryByTagsDialog : public CDialogImpl<QueryByTagsDialog> {
	public:
		QueryByTagsDialog(pfc::list_t<metadb_handle_ptr> p_tracks, pfc::string8& p_artist, pfc::string8& p_album) : m_tracks(p_tracks), m_artist(p_artist), m_album(p_album)
		{
			Create(core_api::get_main_window());
		}

		BEGIN_MSG_MAP(QueryByTagsDialog)
			MSG_WM_INITDIALOG(OnInitDialog)
			MSG_WM_CLOSE(OnClose)
			COMMAND_CODE_HANDLER_EX(EN_UPDATE, OnUpdate)
			COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
			COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		END_MSG_MAP()

		enum { IDD = IDD_CUSTOM_QUERY_TAGS };

		BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {
			modeless_dialog_manager::g_add(m_hWnd);
			m_ok = GetDlgItem(IDOK);
			uSetDlgItemText(m_hWnd, IDC_ARTIST, m_artist);
			uSetDlgItemText(m_hWnd, IDC_ALBUM, m_album);
			return true;
		}

		void OnClose() {
			DestroyWindow();
		}

		void OnFinalMessage(HWND hwnd) {
			modeless_dialog_manager::g_remove(m_hWnd);
			delete this;
		}
	
		void OnUpdate(UINT uNotifyCode, int nID, CWindow wndCtl) {
			m_ok.EnableWindow(string_utf8_from_window(m_hWnd, IDC_ARTIST).length() && string_utf8_from_window(m_hWnd, IDC_ALBUM).length());
		}

		void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl) {
			DestroyWindow();
		}

		void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl) {
			pfc::string8 artist = string_utf8_from_window(m_hWnd, IDC_ARTIST);
			pfc::string8 album = string_utf8_from_window(m_hWnd, IDC_ALBUM);
			t_size count = m_tracks.get_count();
			auto query = new Query("release");
			pfc::string8 search;
			search << "artist:\"" << artist << "\"";
			search << " AND release:\"" << album << "\"";
			search << " AND (tracks:" << count;
			search << " OR tracksmedium:" << count << ")";
			query->add_param("query", search);
			query->add_param("limit", 100);
			new TaggerDialog(query, m_tracks);
			DestroyWindow();
		}

	private:
		CButton m_ok;
		pfc::list_t<metadb_handle_ptr> m_tracks;
		pfc::string8 m_artist;
		pfc::string8 m_album;
	};
}
