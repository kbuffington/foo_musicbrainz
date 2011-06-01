#pragma once

#include "FileTagMap.h"
#include "Query.h"
#include "TrackListView.h"
#include "TagWriter.h"
#include "ListView.h"
#include "RequestThread.h"
#include "Release.h"
#include "ReleaseGroup.h"
#include "Track.h"

namespace foo_musicbrainz {
	class TaggerDialog : public CDialogImpl<TaggerDialog> {
	private:
		CListViewCtrl release_list;
		ListView medium_list;
		ListView label_info_listview;
		ListView track_list;
		CComboBox type;
		CComboBox status;
		CEdit artist;
		CEdit album;
		CEdit date;
		CEdit barcode;
		CEdit url;
		ReleaseList *mbc;
		pfc::list_t<metadb_handle_ptr> tracks;
		TrackListView track_list_view;
		bool loaded;
		size_t current_release;
		size_t current_medium;

	public:
		enum { IDD = IDD_TAGGER };

		TaggerDialog(foo_musicbrainz::Query *query, pfc::list_t<metadb_handle_ptr> _tracks) :
			CDialogImpl<TaggerDialog>(),
			tracks(_tracks),
			loaded(false),
			current_release(0),
			current_medium(0)
		{
			mbc = new ReleaseList();
			Create(core_api::get_main_window());
			threaded_process::g_run_modeless(new service_impl_t<foo_musicbrainz::RequestThread>(query, m_hWnd, mbc, &tracks),
				threaded_process::flag_show_progress | threaded_process::flag_show_abort, m_hWnd, "Querying data from MusicBrainz");
		}

		BEGIN_MSG_MAP(TaggerDialog)
			MSG_WM_INITDIALOG(OnInitDialog)
			MSG_WM_SHOWWINDOW(OnShowWindow)
			MSG_WM_CLOSE(OnClose)
			NOTIFY_HANDLER_EX(IDC_RELEASE_LIST, LVN_ITEMCHANGED, OnReleaseListChange)
			NOTIFY_HANDLER_EX(IDC_MEDIUM_LIST, LVN_ITEMCHANGED, OnMediumChange)
			NOTIFY_HANDLER_EX(IDC_TRACK_LIST, NM_CLICK, OnTrackListClick)
			NOTIFY_HANDLER_EX(IDC_URL, NM_CLICK, OnLink)
			NOTIFY_HANDLER_EX(IDC_URL, NM_RETURN, OnLink)
			COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
			COMMAND_ID_HANDLER_EX(IDOK, OnOk)
			COMMAND_HANDLER_EX(IDC_TYPE, CBN_SELENDOK, OnTypeChange)
			COMMAND_HANDLER_EX(IDC_STATUS, CBN_SELENDOK, OnStatusChange)
			COMMAND_HANDLER_EX(IDC_ARTIST, EN_UPDATE, OnArtistUpdate)
			COMMAND_HANDLER_EX(IDC_ALBUM, EN_UPDATE, OnAlbumUpdate)
			COMMAND_HANDLER_EX(IDC_DATE, EN_UPDATE, OnDateUpdate)
			COMMAND_HANDLER_EX(IDC_BARCODE, EN_UPDATE, on_barcode_update)
			//CHAIN_MSG_MAP(CDialogImpl<TaggerDialog>)
		END_MSG_MAP()

		Release *get_current_release() {
			return mbc->get(current_release);
		}

		Medium *get_current_medium() {
			return get_current_release()->get_medium(current_medium);
		}

		bool OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {
			static_api_ptr_t<modeless_dialog_manager>()->add(m_hWnd);
			release_list = GetDlgItem(IDC_RELEASE_LIST);
			medium_list = GetDlgItem(IDC_MEDIUM_LIST);
			label_info_listview = GetDlgItem(IDC_LABEL_INFO_LIST);
			track_list = GetDlgItem(IDC_TRACK_LIST);
			type = GetDlgItem(IDC_TYPE);
			status = GetDlgItem(IDC_STATUS);
			url = GetDlgItem(IDC_URL);
			artist = GetDlgItem(IDC_ARTIST);
			album = GetDlgItem(IDC_ALBUM);
			date = GetDlgItem(IDC_DATE);
			barcode = GetDlgItem(IDC_BARCODE);
			track_list_view.Attach(track_list);
		
			// List view styles
			auto styles = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP;
			release_list.SetExtendedListViewStyle(styles, styles);
			label_info_listview.SetExtendedListViewStyle(styles, styles);
			medium_list.SetExtendedListViewStyle(styles, styles);
			track_list.SetExtendedListViewStyle(styles, styles);

			// Adding release list columns
			listview_helper::insert_column(release_list, 0, "Artist", 104);
			listview_helper::insert_column(release_list, 1, "Release", 110);
			listview_helper::insert_column(release_list, 2, "Date", 45);
			listview_helper::insert_column(release_list, 3, "Label/Cat#", 80);

			// Medium list columns
			listview_helper::insert_column(medium_list, 0, "", 0); // Fake column
			medium_list.InsertColumn(1, L"#", LVCFMT_RIGHT, 30);
			medium_list.DeleteColumn(0);
			listview_helper::insert_column(medium_list, 1, "Title", 49);

			// Event list columns
			listview_helper::insert_column(label_info_listview, 0, "Label", 80);
			listview_helper::insert_column(label_info_listview, 1, "Cat#", 80);

			// Adding track list columns
			listview_helper::insert_column(track_list, 0, "", 0); // Fake column
			track_list.InsertColumn(1, L"#", LVCFMT_RIGHT, 30);
			track_list.DeleteColumn(0);
			listview_helper::insert_column(track_list, 1, "Title", 260);

			for (auto i = 0; i < ReleaseGroup::type_count; i++) {
				pfc::stringcvt::string_os_from_utf8 str(ReleaseGroup::types[i]);
				type.AddString(str);
			}
			for (auto i = 0; i < Release::status_count; i++) {
				pfc::stringcvt::string_os_from_utf8 str(Release::statuses[i]);
				status.AddString(str);
			}
			return true;
		}

		void OnShowWindow(BOOL bShow, UINT nStatus) {
			SetMsgHandled(FALSE);
			if (loaded) return;
			loaded = true;
			
			for (unsigned int i = 0; i < mbc->count(); i++) {
				auto release = mbc->get(i);
				// Artist
				listview_helper::insert_item(release_list, i, release->get_artist_credit()->get_name(), NULL);
				// Title
				listview_helper::set_item_text(release_list, i, 1, release->get_title());
				// Date
				listview_helper::set_item_text(release_list, i, 2, static_cast<pfc::string8>(release->get_date()));
				// Label
				if (release->label_info_count() > 0) {
					auto label_info = release->get_label_info(0);
					auto label = label_info->get_label()->get_name();
					auto cat = label_info->get_catalog_number();
					pfc::string8 labelcat;
					labelcat << label;
					if (!cat.is_empty()) labelcat << " / " << cat;
					listview_helper::set_item_text(release_list, i, 3, labelcat);
				} else {
					listview_helper::set_item_text(release_list, i, 3, "-");
				}
			}

			UpdateRelease();
		}

		void UpdateRelease() {
			Release *release = get_current_release();

			// Artist
			uSetWindowText(artist, release->get_artist_credit()->get_name());
			// Album
			uSetWindowText(album, release->get_title());
			// Date
			uSetWindowText(date, static_cast<pfc::string8>(release->get_date()));
			// Barcode
			uSetWindowText(barcode, release->get_barcode());
			// Type
			type.SetCurSel(get_current_release()->get_release_group()->get_type_index());
			// Status
			status.SetCurSel(get_current_release()->get_status_index());

			// VA?
			if (track_list.GetColumnWidth(2) && !release->is_various()) {
				track_list.SetColumnWidth(1, 260);
				track_list.DeleteColumn(2);
			} else if (!track_list.GetColumnWidth(2) && release->is_various()) {
				track_list.SetColumnWidth(1, 195);
				listview_helper::insert_column(track_list, 2, "Track artist", 130);
			}

			// Media
			medium_list.Resize(release->medium_count());
			for (size_t item = 0; item < release->medium_count(); item++) {
				pfc::string8 position;
				position << release->get_medium(item)->get_position();
				listview_helper::set_item_text(medium_list, item, 0, position);
				listview_helper::set_item_text(medium_list, item, 1, release->get_medium(item)->get_title());
			}
			if (current_medium >= release->medium_count()) {
				current_medium = 0;
			}

			// Labels
			label_info_listview.Resize(release->label_info_count());
			for (size_t i = 0; i < release->label_info_count(); i++) {
				auto label_item = release->get_label_info(i);
				listview_helper::set_item_text(label_info_listview, i, 0, label_item->get_label()->get_name());
				listview_helper::set_item_text(label_info_listview, i, 1, label_item->get_catalog_number());
			}

			// Link
			pfc::string8 url_string = "<a href=\"http://musicbrainz.org/release/";
			url_string += release->get_id();
			url_string += "\">MusicBrainz release page</a>";
			uSetWindowText(url, url_string);

			// Tracks
			update_tracks();
		}

		void update_tracks() {
			auto medium = get_current_medium();
			track_list_view.set_medium(medium);
			track_list.Resize(medium->track_count());
			for (size_t item = 0; item < medium->track_count(); item++) {
				pfc::string8 position;
				position << medium->get_track(item)->get_position();
				listview_helper::set_item_text(track_list, item, 0, position);
				auto track = medium->get_track(item);
				listview_helper::set_item_text(track_list, item, 1, track->get_title());
				listview_helper::set_item_text(track_list, item, 2, track->get_artist_credit()->get_name());
			}
		}

		LRESULT OnTrackListClick(LPNMHDR pnmh) {
			if (((LPNMITEMACTIVATE)pnmh)->iItem != -1 && ((LPNMITEMACTIVATE)pnmh)->iSubItem != 0) {
				track_list_view.Start(((LPNMITEMACTIVATE)pnmh)->iItem, ((LPNMITEMACTIVATE)pnmh)->iSubItem);
			}
			return 0;
		}

		LRESULT OnMediumChange(LPNMHDR pnmh) {
			if (((LPNMLISTVIEW)pnmh)->iItem != -1 && ((LPNMLISTVIEW)pnmh)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)pnmh)->uNewState & LVIS_SELECTED) {
				if (current_medium != ((LPNMITEMACTIVATE)pnmh)->iItem) {
					if (track_list_view.IsActive()) track_list_view.Abort();
					current_medium = ((LPNMITEMACTIVATE)pnmh)->iItem;
					update_tracks();
				}
			}
			return 0;
		}

		LRESULT OnReleaseListChange(LPNMHDR pnmh) {
			if (((LPNMLISTVIEW)pnmh)->iItem != -1 && ((LPNMLISTVIEW)pnmh)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)pnmh)->uNewState & LVIS_SELECTED) {
				if (current_release != ((LPNMITEMACTIVATE)pnmh)->iItem) {
					if (track_list_view.IsActive()) track_list_view.Abort();
					current_release = ((LPNMITEMACTIVATE)pnmh)->iItem;
					UpdateRelease();
				}
			}
			return 0;
		}

		LRESULT OnLink(LPNMHDR pnmh) {
			ShellExecute(NULL, L"open", ((PNMLINK)pnmh)->item.szUrl, NULL, NULL, SW_SHOW);
			return 0;
		}

		void OnClose() {
			DestroyWindow();
		}

		void OnFinalMessage(HWND hwnd) {
			static_api_ptr_t<modeless_dialog_manager>()->remove(m_hWnd);
			delete mbc;
			delete this;
		}

		void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl) {
			DestroyWindow();
		}

		void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl) {
			FileTagMap file_tag_map(*get_current_release(), tracks, current_medium);
			static_api_ptr_t<metadb_io_v2>()->update_info_async(tracks,
				new service_impl_t<TagWriter>(file_tag_map),
				core_api::get_main_window(), metadb_io_v2::op_flag_delay_ui, nullptr);
			DestroyWindow();
		}

		void OnTypeChange(UINT uNotifyCode, int nID, CWindow wndCtl) {
			get_current_release()->get_release_group()->set_type(type.GetCurSel());
		}

		void OnStatusChange(UINT uNotifyCode, int nID, CWindow wndCtl) {
			get_current_release()->set_status(status.GetCurSel());
		}

		void OnArtistUpdate(UINT uNotifyCode, int nID, CWindow wndCtl) {
			pfc::string8 str;
			uGetWindowText(artist, str);
			get_current_release()->get_artist_credit()->set_name(str);
			listview_helper::set_item_text(release_list, current_release, 0, str);
		}

		void OnAlbumUpdate(UINT uNotifyCode, int nID, CWindow wndCtl) {
			pfc::string8 str;
			uGetWindowText(album, str);
			get_current_release()->set_title(str);
			listview_helper::set_item_text(release_list, current_release, 1, str);
		}

		void OnDateUpdate(UINT uNotifyCode, int nID, CWindow wndCtl) {
			pfc::string8 str;
			uGetWindowText(date, str);
			get_current_release()->set_date(Date(str));
			listview_helper::set_item_text(release_list, current_release, 2,
				static_cast<pfc::string8>(get_current_release()->get_date()));
		}

		void on_barcode_update(UINT, int, CWindow) {
			pfc::string8 barcode_value;
			uGetWindowText(barcode, barcode_value);
			get_current_release()->set_barcode(barcode_value);
		}
	};
}
