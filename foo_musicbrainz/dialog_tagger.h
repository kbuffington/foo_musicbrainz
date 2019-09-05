#pragma once

class dialog_tagger : public CDialogImpl<dialog_tagger>, private IListControlOwnerDataSource
{
public:
	dialog_tagger(const pfc::list_t<Release>& p_release_list , metadb_handle_list_cref p_handles) :
		m_release_list(p_release_list),
		m_handles(p_handles),
		track_list(this),
		current_release(0),
		current_disc(0)
	{}

	BEGIN_MSG_MAP(dialog_tagger)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		NOTIFY_HANDLER_EX(IDC_RELEASE_LIST, LVN_ITEMCHANGED, OnReleaseListChange)
		NOTIFY_HANDLER_EX(IDC_URL, NM_CLICK, OnLink)
		NOTIFY_HANDLER_EX(IDC_URL, NM_RETURN, OnLink)

		COMMAND_HANDLER_EX(IDC_TYPE, CBN_SELENDOK, OnTypeChange)
		COMMAND_HANDLER_EX(IDC_STATUS, CBN_SELENDOK, OnStatusChange)
		COMMAND_HANDLER_EX(IDC_DISC, CBN_SELENDOK, OnDiscChange)

		COMMAND_HANDLER_EX(IDC_ARTIST, EN_UPDATE, OnArtistUpdate)
		COMMAND_HANDLER_EX(IDC_ALBUM, EN_UPDATE, OnAlbumUpdate)
		COMMAND_HANDLER_EX(IDC_BARCODE, EN_UPDATE, OnBarcodeUpdate)
		COMMAND_HANDLER_EX(IDC_DATE, EN_UPDATE, OnDateUpdate)
		COMMAND_HANDLER_EX(IDC_FIRST_RELEASE_DATE, EN_UPDATE, OnFirstDateUpdate)
		COMMAND_HANDLER_EX(IDC_SUBTITLE, EN_UPDATE, OnSubtitleUpdate)

		COMMAND_RANGE_HANDLER_EX(IDC_LABEL, IDC_CATALOG, OnLabelCatalogUpdate)
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
		CenterWindow();

		release_list = GetDlgItem(IDC_RELEASE_LIST);
		track_list.CreateInDialog(*this, IDC_TRACK_LIST);
		
		type = GetDlgItem(IDC_TYPE);
		status = GetDlgItem(IDC_STATUS);
		disc = GetDlgItem(IDC_DISC);

		album_artist = GetDlgItem(IDC_ARTIST);
		album = GetDlgItem(IDC_ALBUM);
		date = GetDlgItem(IDC_DATE);
		first_release_date = GetDlgItem(IDC_FIRST_RELEASE_DATE);
		barcode = GetDlgItem(IDC_BARCODE);
		subtitle = GetDlgItem(IDC_SUBTITLE);
		label = GetDlgItem(IDC_LABEL);
		catalog = GetDlgItem(IDC_CATALOG);

		url = GetDlgItem(IDC_URL);
		
		// List view styles
		auto styles = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP;
		release_list.SetExtendedListViewStyle(styles, styles);

		// Adding release list columns
		listview_helper::insert_column(release_list, artist_column, "Artist", 120);
		listview_helper::insert_column(release_list, release_column, "Release", 120);
		listview_helper::insert_column(release_list, date_column, "Date/Country", 80);
		listview_helper::insert_column(release_list, label_column, "Label/Cat#", 140);
		listview_helper::insert_column(release_list, format_column, "Format", 70);

		// Adding track list columns
		auto DPI = track_list.GetDPI();
		track_list.AddColumn("#", MulDiv(30, DPI.cx, 96));
		track_list.AddColumn("Title", MulDiv(250, DPI.cx, 96));
		track_list.AddColumn("Track Artist", MulDiv(170, DPI.cx, 96));

		for (const auto& i : release_group_types)
		{
			type.AddString(string_wide_from_utf8_fast(i));
		}

		for (const auto& i : release_statuses)
		{
			status.AddString(string_wide_from_utf8_fast(i));
		}
		
		for (t_size i = 0; i < m_handles.get_count(); ++i)
		{
			listview_helper::insert_item(track_list, i, PFC_string_formatter() << (i + 1), NULL);
		}

		for (t_size i = 0; i < m_release_list.get_count(); i++)
		{
			auto& release = m_release_list[i];
			// Artist
			listview_helper::insert_item(release_list, i, release.album_artist, NULL);
			// Title
			listview_helper::set_item_text(release_list, i, release_column, release.title);
			// Date / Country
			listview_helper::set_item_text(release_list, i, date_column, slasher(release.date, release.country));
			// Label
			listview_helper::set_item_text(release_list, i, label_column, slasher(release.label, release.catalognumber));
			// Format
			listview_helper::set_item_text(release_list, i, format_column, release.discs[0].format);
		}

		UpdateRelease();
		return true;
	}

	LRESULT OnReleaseListChange(LPNMHDR pnmh)
	{
		int new_index = ((LPNMLISTVIEW)pnmh)->iItem;
		if (new_index != -1 && ((LPNMLISTVIEW)pnmh)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)pnmh)->uNewState & LVIS_SELECTED && current_release != new_index)
		{
			if (track_list.TableEdit_IsActive())
			{
				track_list.TableEdit_Abort(true);
			}
			current_release = new_index;
			UpdateRelease();
		}
		return 0;
	}

	void OnTypeChange(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		m_release_list[current_release].primary_type = get_type_str(type.GetCurSel());
	}

	void OnStatusChange(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		m_release_list[current_release].status = get_status_str(status.GetCurSel());
	}

	void OnDiscChange(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		current_disc = disc.GetCurSel();
		UpdateDisc();
	}

	LRESULT OnLink(LPNMHDR pnmh)
	{
		ShellExecute(NULL, L"open", ((PNMLINK)pnmh)->item.szUrl, NULL, NULL, SW_SHOW);
		return 0;
	}

	void UpdateRelease()
	{
		auto release = m_release_list[current_release];

		uSetWindowText(album_artist, release.album_artist);
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
		auto& release = m_release_list[current_release];
		subtitle.EnableWindow(release.discs.get_count() > 1);
		auto& d = release.discs[current_disc];
		uSetWindowText(subtitle, d.subtitle);

		disc.ResetContent();
		for (t_size i = 0; i < release.discs.get_count(); ++i)
		{
			disc.AddString(string_wide_from_utf8_fast(PFC_string_formatter() << "Disc " << release.discs[i].disc << " of " << release.discs[i].totaldiscs));
		}
		disc.SetCurSel(current_disc);

		// Tracks
		track_list.ReloadData();
	}

	void OnClose()
	{
		DestroyWindow();
	}

	void OnCancel(UINT, int, CWindow)
	{
		DestroyWindow();
	}

	void OnOk(UINT, int, CWindow)
	{
		DestroyWindow();
	}

	void OnArtistUpdate(UINT, int, CWindow)
	{
		uGetWindowText(album_artist, m_release_list[current_release].album_artist);
		listview_helper::set_item_text(release_list, current_release, artist_column, m_release_list[current_release].album_artist);
	}

	void OnAlbumUpdate(UINT, int, CWindow)
	{
		uGetWindowText(album, m_release_list[current_release].title);
		listview_helper::set_item_text(release_list, current_release, release_column, m_release_list[current_release].title);
	}

	void OnDateUpdate(UINT, int, CWindow)
	{
		uGetWindowText(date, m_release_list[current_release].date);

		str8 date_country = PFC_string_formatter() << m_release_list[current_release].date << "/" << m_release_list[current_release].country;
		if (strcmp(date_country, "/") == 0)
			date_country = "-";
		listview_helper::set_item_text(release_list, current_release, date_column, date_country);
	}

	void OnFirstDateUpdate(UINT, int, CWindow)
	{
		uGetWindowText(first_release_date, m_release_list[current_release].first_release_date);
	}

	void OnBarcodeUpdate(UINT, int, CWindow)
	{
		uGetWindowText(barcode, m_release_list[current_release].barcode);
	}

	void OnSubtitleUpdate(UINT, int, CWindow)
	{
		uGetWindowText(barcode, m_release_list[current_release].discs[current_disc].subtitle);
	}

	void OnLabelCatalogUpdate(UINT, int, CWindow)
	{
		uGetWindowText(label, m_release_list[current_release].label);
		uGetWindowText(catalog, m_release_list[current_release].catalognumber);
		listview_helper::set_item_text(release_list, current_release, label_column, slasher(m_release_list[current_release].label, m_release_list[current_release].catalognumber));
	}

private:
	// IListControlOwnerDataSource methods

	bool listIsColumnEditable(ctx_t, size_t sub_item) override
	{ 
		return sub_item > 0;
	}

	str8 listGetSubItemText(ctx_t, t_size item, t_size sub_item) override
	{
		auto& tracks = m_release_list[current_release].discs[current_disc].tracks;
		switch (sub_item)
		{
		case 0:
			return tracks[item].track;
		case 1:
			return tracks[item].title;
		case 2:
			return tracks[item].artist;
		default:
			return "";
		}
	}

	t_size listGetItemCount(ctx_t ctx) override
	{
		return m_handles.get_count();
	}

	void listSubItemClicked(ctx_t, t_size item, t_size sub_item) override
	{
		if (sub_item > 0)
		{
			track_list.TableEdit_Start(item, sub_item);
		}
	}

	void listSetEditField(ctx_t ctx, t_size item, t_size sub_item, const char* value) override
	{
		switch (sub_item)
		{
		case 1:
			m_release_list[current_release].discs[current_disc].tracks[item].title = value;
			break;
		case 2:
			m_release_list[current_release].discs[current_disc].tracks[item].artist = value;
			break;
		}
	}

	CComboBox disc;
	CComboBox type;
	CComboBox status;
	CEdit album_artist;
	CEdit album;
	CEdit date;
	CEdit first_release_date;
	CEdit barcode;
	CEdit url;
	CEdit subtitle;
	CEdit label;
	CEdit catalog;
	CListViewCtrl release_list;
	CListControlOwnerData track_list;
	metadb_handle_list m_handles;
	pfc::list_t<Release> m_release_list;
	t_size current_release;
	t_size current_disc;
};
