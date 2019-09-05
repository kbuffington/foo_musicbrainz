#pragma once

class dialog_tagger : public CDialogImpl<dialog_tagger> {
public:
	dialog_tagger(const pfc::list_t<Release>& p_release_list , metadb_handle_list_cref p_handles) :
		m_release_list(p_release_list),
		m_handles(p_handles),
		current_release(0),
		current_disc(0)
	{
		Create(core_api::get_main_window());
	}

	BEGIN_MSG_MAP(dialog_tagger)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		NOTIFY_HANDLER_EX(IDC_RELEASE_LIST, LVN_ITEMCHANGED, OnReleaseListChange)
		NOTIFY_HANDLER_EX(IDC_TRACK_LIST, NM_CLICK, OnTrackListClick)
		NOTIFY_HANDLER_EX(IDC_URL, NM_CLICK, OnLink)
		NOTIFY_HANDLER_EX(IDC_URL, NM_RETURN, OnLink)
		COMMAND_HANDLER_EX(IDC_TYPE, CBN_SELENDOK, OnTypeChange)
		COMMAND_HANDLER_EX(IDC_STATUS, CBN_SELENDOK, OnStatusChange)
		COMMAND_HANDLER_EX(IDC_DISC_LIST, CBN_SELCHANGE, OnDiscUpdate)
		COMMAND_HANDLER_EX(IDC_ARTIST, EN_UPDATE, OnArtistUpdate)
		COMMAND_HANDLER_EX(IDC_ALBUM, EN_UPDATE, OnAlbumUpdate)
		COMMAND_HANDLER_EX(IDC_BARCODE, EN_UPDATE, OnBarcodeUpdate)
		COMMAND_HANDLER_EX(IDC_CATALOG, EN_UPDATE, OnCatalogUpdate)
		COMMAND_HANDLER_EX(IDC_DATE, EN_UPDATE, OnDateUpdate)
		COMMAND_HANDLER_EX(IDC_FIRST_RELEASE_DATE, EN_UPDATE, OnFirstDateUpdate)
		COMMAND_HANDLER_EX(IDC_LABEL, EN_UPDATE, OnLabelUpdate)
		COMMAND_HANDLER_EX(IDC_SUBTITLE, EN_UPDATE, OnSubtitleUpdate)
	END_MSG_MAP()

	enum { IDD = IDD_TAGGER };

	enum columns
	{
		artist_column,
		release_column,
		date_column,
		label_column,
		format_column
	};

	bool OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		modeless_dialog_manager::g_add(m_hWnd);
		release_list = GetDlgItem(IDC_RELEASE_LIST);
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

		track_list_view.attach(track_list);

		// List view styles
		auto styles = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP;
		release_list.SetExtendedListViewStyle(styles, styles);
		track_list.SetExtendedListViewStyle(styles, styles);

		// Adding release list columns
		listview_helper::insert_column(release_list, artist_column, "Artist", 120);
		listview_helper::insert_column(release_list, release_column, "Release", 120);
		listview_helper::insert_column(release_list, date_column, "Date/Country", 80);
		listview_helper::insert_column(release_list, label_column, "Label/Cat#", 140);
		listview_helper::insert_column(release_list, format_column, "Format", 70);

		// Adding track list columns
		listview_helper::insert_column(track_list, 0, "", 0); // Fake column
		track_list.InsertColumn(1, L"#", LVCFMT_RIGHT, 30);
		track_list.DeleteColumn(0);
		listview_helper::insert_column(track_list, 1, "Title", 260);

		
		for (const auto& i : release_group_types)
		{
			type.AddString(string_wide_from_utf8_fast(i));
		}
		for (const auto& i : release_statuses)
		{
			status.AddString(string_wide_from_utf8_fast(i));
		}
		
		for (t_size i = 0; i < m_release_list.get_count(); i++)
		{
			auto release = m_release_list.get_item(i);
			// Artist
			listview_helper::insert_item(release_list, i, release.album_artist, NULL);
			// Title
			listview_helper::set_item_text(release_list, i, release_column, release.title);
			// Date / Country
			str8 date_country = release.date << "/" << release.country;
			if (strcmp(date_country, "/") == 0)
				date_country = "-";
			listview_helper::set_item_text(release_list, i, date_column, date_country);
			// Label
			str8 label_catalog = release.label << "/" << release.catalognumber;
			if (strcmp(label_catalog, "/") == 0)
				label_catalog = "-";
			listview_helper::set_item_text(release_list, i, label_column, label_catalog);
			// Format
			listview_helper::set_item_text(release_list, i, format_column, release.discs[0].format);
		}

		UpdateRelease();

		CenterWindow();

		return true;
	}

	LRESULT OnReleaseListChange(LPNMHDR pnmh)
	{
		int new_index = ((LPNMLISTVIEW)pnmh)->iItem;
		if (new_index != -1 && ((LPNMLISTVIEW)pnmh)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)pnmh)->uNewState & LVIS_SELECTED && current_release != new_index)
		{
			if (track_list_view.is_active()) track_list_view.abort();
			current_release = new_index;
			UpdateRelease();
		}
		return 0;
	}

	LRESULT OnTrackListClick(LPNMHDR pnmh)
	{
		int item = ((LPNMITEMACTIVATE)pnmh)->iItem;
		int sub_item = ((LPNMITEMACTIVATE)pnmh)->iSubItem;
		if (item != -1 && sub_item != 0) {
			track_list_view.start(item, sub_item);
		}
		return 0;
	}

	LRESULT OnDiscUpdate(UINT, int, CWindow)
	{
		return 0;
	}

	LRESULT OnLink(LPNMHDR pnmh)
	{
		ShellExecute(NULL, L"open", ((PNMLINK)pnmh)->item.szUrl, NULL, NULL, SW_SHOW);
		return 0;
	}

	void UpdateRelease()
	{
		auto release = m_release_list.get_item(current_release);

		uSetWindowText(artist, release.album_artist);
		uSetWindowText(album, release.title);
		uSetWindowText(date, release.date);
		uSetWindowText(first_release_date, release.first_release_date);
		uSetWindowText(barcode, release.barcode);
		uSetWindowText(label, release.label);
		uSetWindowText(catalog, release.catalognumber);

		type.SetCurSel(get_type_index(release.primary_type));
		status.SetCurSel(get_status_index(release.status));

		str8 url_str = PFC_string_formatter() << "<a href=\"https://musicbrainz.org/release/" << release.albumid << "\">MusicBrainz release page</a>";
		uSetWindowText(url, url_str);

		UpdateDisc();
	}

	void UpdateDisc()
	{
		auto release = m_release_list.get_item(current_release);
		discsubtitle.EnableWindow(release.discs.get_count() > 1);
		auto disc = release.discs[current_disc];
		uSetWindowText(discsubtitle, disc.title);

		// Tracks
		//UpdateTracks();
	}

	void OnClose()
	{
		DestroyWindow();
	}

	void OnFinalMessage(HWND hwnd) override
	{
		modeless_dialog_manager::g_remove(m_hWnd);
		delete this;
	}

	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		DestroyWindow();
	}

	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		DestroyWindow();
	}

	void OnTypeChange(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
	}

	void OnStatusChange(UINT uNotifyCode, int nID, CWindow wndCtl)
	{

	}

	void OnArtistUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{

	}

	void OnAlbumUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{

	}

	void OnDateUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{

	}

	void OnFirstDateUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{

	}

	void OnBarcodeUpdate(UINT, int, CWindow)
	{

	}

	void OnSubtitleUpdate(UINT, int, CWindow)
	{

	}

	void OnCatalogUpdate(UINT, int, CWindow)
	{

	}

	void OnLabelUpdate(UINT, int, CWindow)
	{

	}

private:
	CButton groupbox;
	CComboBox disc_list;
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
	CListViewCtrl release_list;
	CListViewCtrl track_list;
	metadb_handle_list m_handles;
	mb_track_list_view track_list_view;
	pfc::list_t<Release> m_release_list;
	t_size current_release;
	t_size current_disc;
};
