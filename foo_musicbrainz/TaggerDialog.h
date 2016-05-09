#pragma once

#include "FileTagMap.h"
#include "Query.h"
#include "TrackListView.h"
#include "TagWriter.h"
#include "ComboBox.h"
#include "ListView.h"
#include "RequestThread.h"
#include "Release.h"
#include "ReleaseGroup.h"
#include "Track.h"

namespace foo_musicbrainz {
	class TaggerDialog : public CDialogImpl<TaggerDialog> {
	private:
		CListViewCtrl release_list;
		ComboBox medium_list;
		ComboBox label_info_list;
		ListView track_list;
		CComboBox type;
		CComboBox status;
		CEdit artist;
		CEdit album;
		CEdit date;
		CEdit first_release_date;
		CEdit barcode;
		CEdit url;
		CEdit discsubtitle;
		CEdit label;
		CEdit catalog;
		CButton groupbox;
		ReleaseList *mbc;
		pfc::list_t<metadb_handle_ptr> tracks;
		TrackListView track_list_view;
		bool loaded;
		size_t current_release;
		size_t current_medium;
		size_t current_label_info;

	public:
		enum { IDD = IDD_TAGGER };

		enum columns {
			artist_column,
			release_column,
			date_column,
			label_column
		};

		TaggerDialog(foo_musicbrainz::Query *query, pfc::list_t<metadb_handle_ptr> _tracks) :
			CDialogImpl<TaggerDialog>(),
			tracks(_tracks),
			loaded(false),
			current_release(0),
			current_medium(0),
			current_label_info(0)
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
			COMMAND_HANDLER_EX(IDC_FIRST_RELEASE_DATE, EN_UPDATE, on_first_release_date_update)
			COMMAND_HANDLER_EX(IDC_BARCODE, EN_UPDATE, on_barcode_update)
			COMMAND_HANDLER_EX(IDC_MEDIUM_LIST, CBN_SELCHANGE, on_medium_change)
			COMMAND_HANDLER_EX(IDC_LABEL_INFO_LIST, CBN_SELCHANGE, on_label_info_change)
			COMMAND_HANDLER_EX(IDC_SUBTITLE, EN_UPDATE, on_disc_subtitle_update)
			COMMAND_HANDLER_EX(IDC_LABEL, EN_UPDATE, on_label_info_update)
			COMMAND_HANDLER_EX(IDC_CATALOG, EN_UPDATE, on_label_info_update)
			//CHAIN_MSG_MAP(CDialogImpl<TaggerDialog>)
		END_MSG_MAP()

		Release *get_current_release() {
			return mbc->get(current_release);
		}

		Medium *get_current_medium() {
			return get_current_release()->get_medium(current_medium);
		}

		LabelInfo *get_current_label_info() {
			return get_current_release()->get_label_info(current_label_info);
		}

		bool OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {
			static_api_ptr_t<modeless_dialog_manager>()->add(m_hWnd);
			release_list = GetDlgItem(IDC_RELEASE_LIST);
			medium_list.Attach(GetDlgItem(IDC_MEDIUM_LIST));
			label_info_list.Attach(GetDlgItem(IDC_LABEL_INFO_LIST));
			track_list = GetDlgItem(IDC_TRACK_LIST);
			type = GetDlgItem(IDC_TYPE);
			status = GetDlgItem(IDC_STATUS);
			url = GetDlgItem(IDC_URL);
			artist = GetDlgItem(IDC_ARTIST);
			album = GetDlgItem(IDC_ALBUM);
			date = GetDlgItem(IDC_DATE);
			first_release_date = GetDlgItem(IDC_FIRST_RELEASE_DATE);
			barcode = GetDlgItem(IDC_BARCODE);
			discsubtitle = GetDlgItem(IDC_SUBTITLE);
			label = GetDlgItem(IDC_LABEL);
			catalog = GetDlgItem(IDC_CATALOG);
			groupbox = GetDlgItem(IDC_CHOOSE_DISC);
			track_list_view.Attach(track_list);

			// List view styles
			auto styles = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP;
			release_list.SetExtendedListViewStyle(styles, styles);
			track_list.SetExtendedListViewStyle(styles, styles);

			// Adding release list columns
			listview_helper::insert_column(release_list, artist_column, "Artist", 80);
			listview_helper::insert_column(release_list, release_column, "Release", 80);
			listview_helper::insert_column(release_list, date_column, "Date/Country", 60);
			listview_helper::insert_column(release_list, label_column, "Label/Cat#", 110);

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
				listview_helper::set_item_text(release_list, i, release_column, release->get_title());
				// Date / Country
				pfc::string8 date_country = static_cast<pfc::string8>(release->get_date()) << "/" << release->get_country();
				if (strcmp(date_country, "/") == 0)
					date_country = "-";
				listview_helper::set_item_text(release_list, i, date_column, date_country);
				// Label
				if (release->label_info_count() == 0) {
					release->add_label_info(new LabelInfo());
				}
				listview_helper::set_item_text(release_list, i, label_column, release->get_label_info(0)->get_info());
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
			// First release date
			uSetWindowText(first_release_date, static_cast<pfc::string8>(release->get_release_group()->get_first_release_date()));
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
			medium_list.DeleteAll();
			for (size_t i = 0; i < release->medium_count(); i++) {
				auto info = release->get_medium(i)->get_info();
				medium_list.AddString(pfc::stringcvt::string_os_from_utf8(info));
			}
			if (current_medium >= release->medium_count()) {
				current_medium = 0;
			}
			medium_list.SetCurSel(current_medium);
			update_medium();

			// "Choose disc" 
			if (release->track_count() > tracks.get_count()) {
				uSetWindowText(groupbox, "Discs (choose one)");
			} else {
				uSetWindowText(groupbox, "Discs");
			}

			// Labels
			label_info_list.DeleteAll();
			for (size_t i = 0; i < release->label_info_count(); i++) {
				auto info = release->get_label_info(i)->get_info();
				label_info_list.AddString(pfc::stringcvt::string_os_from_utf8(info));
			} 
			current_label_info = 0;
			label_info_list.SetCurSel(0);
			update_label_info();

			// Link
			pfc::string8 url_string = "<a href=\"http://musicbrainz.org/release/";
			url_string += release->get_id();
			url_string += "\">MusicBrainz release page</a>";
			uSetWindowText(url, url_string);
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

		void update_medium() {
			auto release = get_current_release();
			auto medium = get_current_medium();
			uSetWindowText(discsubtitle, medium->get_title());

			// Tracks
			update_tracks();
		}

		void update_label_info() {
			auto label_info = get_current_label_info();
			uSetWindowText(label, label_info->get_name());
			uSetWindowText(catalog, label_info->get_catalog_number());
		}

		LRESULT OnTrackListClick(LPNMHDR pnmh) {
			if (((LPNMITEMACTIVATE)pnmh)->iItem != -1 && ((LPNMITEMACTIVATE)pnmh)->iSubItem != 0) {
				track_list_view.Start(((LPNMITEMACTIVATE)pnmh)->iItem, ((LPNMITEMACTIVATE)pnmh)->iSubItem);
			}
			return 0;
		}

		LRESULT on_medium_change(UINT, int, CWindow) {
			current_medium = medium_list.GetCurSel();
			update_medium();
			return 0;
		}

		LRESULT on_label_info_change(UINT, int, CWindow) {
			current_label_info = label_info_list.GetCurSel();
			update_label_info();
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
			listview_helper::set_item_text(release_list, current_release, artist_column, str);
		}

		void OnAlbumUpdate(UINT uNotifyCode, int nID, CWindow wndCtl) {
			pfc::string8 str;
			uGetWindowText(album, str);
			get_current_release()->set_title(str);
			listview_helper::set_item_text(release_list, current_release, release_column, str);
		}

		void OnDateUpdate(UINT uNotifyCode, int nID, CWindow wndCtl) {
			pfc::string8 str;
			uGetWindowText(date, str);
			get_current_release()->set_date(Date(str));
			pfc::string8 date_country = static_cast<pfc::string8>(get_current_release()->get_date()) << "/" << get_current_release()->get_country();
			if (strcmp(date_country, "/") == 0)
				date_country = "-";
			listview_helper::set_item_text(release_list, current_release, date_column, date_country);
		}

		void on_first_release_date_update(UINT uNotifyCode, int nID, CWindow wndCtl) {
			pfc::string8 str;
			uGetWindowText(first_release_date, str);
			get_current_release()->get_release_group()->set_first_release_date(Date(str));
		}

		void on_barcode_update(UINT, int, CWindow) {
			pfc::string8 barcode_value;
			uGetWindowText(barcode, barcode_value);
			get_current_release()->set_barcode(barcode_value);
		}

		void on_disc_subtitle_update(UINT, int, CWindow) {
			pfc::string8 str;
			uGetWindowText(discsubtitle, str);
			auto medium = get_current_medium();
			medium->set_title(str);
			pfc::stringcvt::string_os_from_utf8 info(medium->get_info());
			medium_list.SetCurrentString(info);
		}

		void on_label_info_update(UINT, int id, CWindow ctrl) {
			pfc::string8 str;
			uGetWindowText(ctrl, str);
			auto label_info = get_current_label_info();
			switch (id) {
			case IDC_LABEL:
				label_info->set_name(str);
				break;
			case IDC_CATALOG:
				label_info->set_catalog_number(str);
				break;
			}

			pfc::stringcvt::string_os_from_utf8 info(label_info->get_info());
			label_info_list.SetCurrentString(info);
			if (current_label_info == 0) {
				release_list.SetItemText(current_release, label_column, info);
			}
		}
	};
}
