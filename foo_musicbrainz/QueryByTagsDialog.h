#pragma once

#include "foo_musicbrainz.h"
#include "ReleaseList.h"
#include "TaggerDialog.h"

namespace foo_musicbrainz {
	class QueryByTagsDialog : public CDialogImpl<QueryByTagsDialog> {
	private:
		pfc::list_t<metadb_handle_ptr> tracks;
		CButton ok;
		pfc::string8 artist;
		pfc::string8 album;

	public:
		enum { IDD = IDD_CUSTOM_QUERY_TAGS };

		QueryByTagsDialog(pfc::list_t<metadb_handle_ptr> _tracks, pfc::string8 &_artist, pfc::string8 &_album)
			: CDialogImpl<QueryByTagsDialog>(),
			tracks(_tracks),
			artist(_artist),
			album(_album)
		{
			Create(core_api::get_main_window());
		}

		BEGIN_MSG_MAP(CCustomQueryTags)
			MSG_WM_INITDIALOG(OnInitDialog)
			MSG_WM_CLOSE(OnClose)
			COMMAND_CODE_HANDLER_EX(EN_UPDATE, OnUpdate)
			COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
			COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		END_MSG_MAP()

		BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {
			static_api_ptr_t<modeless_dialog_manager>()->add(m_hWnd);
			ok = GetDlgItem(IDOK);
			uSetDlgItemText(m_hWnd, IDC_ARTIST, artist);
			uSetDlgItemText(m_hWnd, IDC_ALBUM, album);
			return true;
		}

		void OnClose() {
			DestroyWindow();
		}

		void OnFinalMessage(HWND hwnd) {
			static_api_ptr_t<modeless_dialog_manager>()->remove(m_hWnd);
			m_hWnd = NULL;
			delete this;
		}
	
		void OnUpdate(UINT uNotifyCode, int nID, CWindow wndCtl) {
			if (string_utf8_from_window(m_hWnd, IDC_ARTIST).is_empty() || string_utf8_from_window(m_hWnd, IDC_ALBUM).is_empty()) {
				ok.EnableWindow(false);
			} else {
				ok.EnableWindow(true);
			}
		}

		void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl) {
			DestroyWindow();
		}

		void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl) {
			pfc::string8 artist = string_utf8_from_window(m_hWnd, IDC_ARTIST);
			pfc::string8 album = string_utf8_from_window(m_hWnd, IDC_ALBUM);
			if (!artist.is_empty() && !album.is_empty()) {
				auto query = new Query("release");
				pfc::string8 search;
				search << "artist:\"" << artist << "\"";
				search << " AND release:\"" << album << "\"";
				search << " AND (tracks:" << tracks.get_count();
				search << " OR tracksmedium:" << tracks.get_count() << ")";
				query->add_param("query", search);
				new TaggerDialog(query, tracks);
				DestroyWindow();
			}
		}
	};
}
