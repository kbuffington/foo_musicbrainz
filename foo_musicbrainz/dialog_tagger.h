#pragma once

class dialog_tagger : public CDialogImpl<dialog_tagger>, private IListControlOwnerDataSource
{
public:
	dialog_tagger(const pfc::list_t<Release>& p_release_list, metadb_handle_list_cref p_handles) :
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
		COMMAND_HANDLER_EX(IDC_DATE, EN_UPDATE, OnDateUpdate)
		COMMAND_HANDLER_EX(IDC_FIRST_RELEASE_DATE, EN_UPDATE, OnFirstDateUpdate)
		COMMAND_HANDLER_EX(IDC_LABEL, EN_UPDATE, OnLabelUpdate)
		COMMAND_HANDLER_EX(IDC_CATALOG, EN_UPDATE, OnCatalogUpdate)
		COMMAND_HANDLER_EX(IDC_BARCODE, EN_UPDATE, OnBarcodeUpdate)
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

	BOOL OnInitDialog(CWindow, LPARAM)
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

		// Add release list columns
		listview_helper::insert_column(release_list, artist_column, "Artist", 120);
		listview_helper::insert_column(release_list, release_column, "Release", 120);
		listview_helper::insert_column(release_list, date_column, "Date/Country", 80);
		listview_helper::insert_column(release_list, label_column, "Label/Cat#", 140);
		listview_helper::insert_column(release_list, format_column, "Format", 70);

		// Add release list rows
		for (t_size i = 0; i < m_release_list.get_count(); i++)
		{
			listview_helper::insert_item(release_list, i, m_release_list[i].album_artist, NULL);
			listview_helper::set_item_text(release_list, i, release_column, m_release_list[i].title);
			listview_helper::set_item_text(release_list, i, date_column, slasher(m_release_list[i].date, m_release_list[i].country));
			listview_helper::set_item_text(release_list, i, label_column, slasher(m_release_list[i].label, m_release_list[i].catalog));
			listview_helper::set_item_text(release_list, i, format_column, m_release_list[i].discs[0].format);
		}

		// Adding track list columns
		auto DPI = track_list.GetDPI();
		track_list.AddColumn("#", MulDiv(30, DPI.cx, 96), HDF_CENTER);
		track_list.AddColumn("Title", MulDiv(250, DPI.cx, 96));

		// Fixed combo boxes
		for (const auto& i : release_group_types)
		{
			type.AddString(string_wide_from_utf8_fast(i));
		}

		for (const auto& i : release_statuses)
		{
			status.AddString(string_wide_from_utf8_fast(i));
		}

		UpdateRelease();
		return TRUE;
	}

	LRESULT OnReleaseListChange(LPNMHDR pnmh)
	{
		int new_index = ((LPNMLISTVIEW)pnmh)->iItem;
		if (new_index != -1 && ((LPNMLISTVIEW)pnmh)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)pnmh)->uNewState & LVIS_SELECTED && current_release != new_index)
		{
			current_release = new_index;
			UpdateRelease();
		}
		return 0;
	}

	void OnTypeChange(UINT, int, CWindow)
	{
		m_release_list[current_release].primary_type = get_type_str(type.GetCurSel());
	}

	void OnStatusChange(UINT, int, CWindow)
	{
		m_release_list[current_release].status = get_status_str(status.GetCurSel());
	}

	void OnDiscChange(UINT, int, CWindow)
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
		uSetWindowText(album_artist, m_release_list[current_release].album_artist);
		uSetWindowText(album, m_release_list[current_release].title);
		uSetWindowText(date, m_release_list[current_release].date);
		uSetWindowText(first_release_date, m_release_list[current_release].first_release_date);
		uSetWindowText(label, m_release_list[current_release].label);
		uSetWindowText(catalog, m_release_list[current_release].catalog);
		uSetWindowText(barcode, m_release_list[current_release].barcode);

		type.SetCurSel(get_type_index(m_release_list[current_release].primary_type));
		status.SetCurSel(get_status_index(m_release_list[current_release].status));
		
		current_disc = 0;
		disc.ResetContent();
		for (t_size i = 0; i < m_release_list[current_release].discs.get_count(); ++i)
		{
			disc.AddString(string_wide_from_utf8_fast(PFC_string_formatter() << "Disc " << m_release_list[current_release].discs[i].disc << " of " << m_release_list[current_release].discs[i].totaldiscs));
		}
		subtitle.EnableWindow(!m_release_list[current_release].discs[0].totaldiscs.equals("1"));
		UpdateDisc();

		str8 url_str = PFC_string_formatter() << "<a href=\"https://musicbrainz.org/release/" << m_release_list[current_release].albumid << "\">MusicBrainz release page</a>";
		uSetWindowText(url, url_str);
	}

	void UpdateDisc()
	{
		disc.SetCurSel(current_disc);
		uSetWindowText(subtitle, m_release_list[current_release].discs[current_disc].subtitle);

		if (track_list.TableEdit_IsActive())
		{
			track_list.TableEdit_Abort(false);
		}

		if (m_release_list[current_release].discs[current_disc].is_various)
		{
			if (track_list.GetColumnCount() == 2)
			{
				auto DPI = track_list.GetDPI();
				track_list.AddColumn("Track Artist", MulDiv(170, DPI.cx, 96));
			}
		}
		else if (track_list.GetColumnCount() == 3)
		{
			track_list.DeleteColumn(2, false);
		}

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
		tagger(m_handles, m_release_list[current_release], current_disc);
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
		listview_helper::set_item_text(release_list, current_release, date_column, slasher(m_release_list[current_release].date, m_release_list[current_release].country));
	}

	void OnFirstDateUpdate(UINT, int, CWindow)
	{
		uGetWindowText(first_release_date, m_release_list[current_release].first_release_date);
	}

	void OnLabelUpdate(UINT, int, CWindow)
	{
		uGetWindowText(label, m_release_list[current_release].label);
		listview_helper::set_item_text(release_list, current_release, label_column, slasher(m_release_list[current_release].label, m_release_list[current_release].catalog));
	}

	void OnCatalogUpdate(UINT, int, CWindow)
	{
		uGetWindowText(catalog, m_release_list[current_release].catalog);
		listview_helper::set_item_text(release_list, current_release, label_column, slasher(m_release_list[current_release].label, m_release_list[current_release].catalog));
	}

	void OnBarcodeUpdate(UINT, int, CWindow)
	{
		uGetWindowText(barcode, m_release_list[current_release].barcode);
	}

	void OnSubtitleUpdate(UINT, int, CWindow)
	{
		uGetWindowText(subtitle, m_release_list[current_release].discs[current_disc].subtitle);
	}

private:
	// IListControlOwnerDataSource methods

	bool listCanSelectItem(ctx_t, t_size) override
	{
		return false;
	}

	bool listIsColumnEditable(ctx_t, t_size sub_item) override
	{
		return sub_item > 0;
	}

	str8 listGetSubItemText(ctx_t, t_size item, t_size sub_item) override
	{
		switch (sub_item)
		{
		case 0:
			return m_release_list[current_release].discs[current_disc].tracks[item].track;
		case 1:
			return m_release_list[current_release].discs[current_disc].tracks[item].title;
		case 2:
			return m_release_list[current_release].discs[current_disc].tracks[item].artist;
		default:
			return "";
		}
	}

	t_size listGetItemCount(ctx_t) override
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

	void listSetEditField(ctx_t, t_size item, t_size sub_item, const char* value) override
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
