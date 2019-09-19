#pragma once

static const CDialogResizeHelper::Param resize_data[] = {
	{ IDOK, 1, 1, 1, 1 },
	{ IDCANCEL, 1, 1, 1, 1 },
	{ IDC_URL, 0, 1, 1, 1 },
	{ IDC_RELEASE_GROUPBOX, 0, 0, 1, 0},
	{ IDC_RELEASE_LIST, 0, 0, 1, 0},
	{ IDC_TRACK_GROUPBOX, 0, 0, 1, 1},
	{ IDC_TRACK_LIST, 0, 0, 1, 1},
	{ IDC_RELEASE_INFO_GROUPBOX, 1, 0, 1, 0},
	{ IDC_ARTIST_LABEL, 1, 0, 1, 0 },
	{ IDC_ARTIST_EDIT, 1, 0, 1, 0 },
	{ IDC_ALBUM_LABEL, 1, 0, 1, 0 },
	{ IDC_ALBUM_EDIT, 1, 0, 1, 0 },
	{ IDC_DATE_LABEL, 1, 0, 1, 0 },
	{ IDC_DATE_EDIT, 1, 0, 1, 0 },
	{ IDC_FIRST_RELEASE_DATE_LABEL, 1, 0, 1, 0 },
	{ IDC_FIRST_RELEASE_DATE_EDIT, 1, 0, 1, 0 },
	{ IDC_LABEL_LABEL, 1, 0, 1, 0 },
	{ IDC_LABEL_EDIT, 1, 0, 1, 0 },
	{ IDC_CATALOG_LABEL, 1, 0, 1, 0 },
	{ IDC_CATALOG_EDIT, 1, 0, 1, 0 },
	{ IDC_BARCODE_LABEL, 1, 0, 1, 0 },
	{ IDC_BARCODE_EDIT, 1, 0, 1, 0 },
	{ IDC_TYPE_LABEL, 1, 0, 1, 0 },
	{ IDC_TYPE_COMBO, 1, 0, 1, 0 },
	{ IDC_STATUS_LABEL, 1, 0, 1, 0 },
	{ IDC_STATUS_COMBO, 1, 0, 1, 0 },
};

static const CRect resize_min_max(660, 425, 0, 0);

class dialog_tagger : public CDialogImpl<dialog_tagger>, private IListControlOwnerDataSource
{
public:
	dialog_tagger(const std::vector<Release>& p_release_list, metadb_handle_list_cref p_handles) :
		m_release_list(p_release_list),
		m_handles(p_handles),
		m_resizer(resize_data, resize_min_max),
		track_list(this),
		current_release(0),
		current_disc(0),
		handle_count(p_handles.get_count())
	{}

	BEGIN_MSG_MAP(dialog_tagger)
		CHAIN_MSG_MAP_MEMBER(m_resizer)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		NOTIFY_HANDLER_EX(IDC_RELEASE_LIST, LVN_ITEMCHANGED, OnReleaseListChange)
		NOTIFY_HANDLER_EX(IDC_URL, NM_CLICK, OnLink)
		NOTIFY_HANDLER_EX(IDC_URL, NM_RETURN, OnLink)

		COMMAND_HANDLER_EX(IDC_TYPE_COMBO, CBN_SELENDOK, OnTypeChange)
		COMMAND_HANDLER_EX(IDC_STATUS_COMBO, CBN_SELENDOK, OnStatusChange)
		COMMAND_HANDLER_EX(IDC_DISC_COMBO, CBN_SELENDOK, OnDiscChange)

		COMMAND_HANDLER_EX(IDC_ARTIST_EDIT, EN_UPDATE, OnArtistUpdate)
		COMMAND_HANDLER_EX(IDC_ALBUM_EDIT, EN_UPDATE, OnAlbumUpdate)
		COMMAND_HANDLER_EX(IDC_DATE_EDIT, EN_UPDATE, OnDateUpdate)
		COMMAND_HANDLER_EX(IDC_FIRST_RELEASE_DATE_EDIT, EN_UPDATE, OnFirstDateUpdate)
		COMMAND_HANDLER_EX(IDC_LABEL_EDIT, EN_UPDATE, OnLabelUpdate)
		COMMAND_HANDLER_EX(IDC_CATALOG_EDIT, EN_UPDATE, OnCatalogUpdate)
		COMMAND_HANDLER_EX(IDC_BARCODE_EDIT, EN_UPDATE, OnBarcodeUpdate)
	END_MSG_MAP()

	enum { IDD = IDD_TAGGER };

	enum columns
	{
		artist_column,
		release_column,
		date_column,
		label_column,
		format_column,
		discs_column
	};

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		CenterWindow();

		release_list = GetDlgItem(IDC_RELEASE_LIST);
		track_list.CreateInDialog(*this, IDC_TRACK_LIST);
		
		type = GetDlgItem(IDC_TYPE_COMBO);
		status = GetDlgItem(IDC_STATUS_COMBO);
		disc = GetDlgItem(IDC_DISC_COMBO);

		album_artist = GetDlgItem(IDC_ARTIST_EDIT);
		album = GetDlgItem(IDC_ALBUM_EDIT);
		date = GetDlgItem(IDC_DATE_EDIT);
		first_release_date = GetDlgItem(IDC_FIRST_RELEASE_DATE_EDIT);
		label = GetDlgItem(IDC_LABEL_EDIT);
		catalog = GetDlgItem(IDC_CATALOG_EDIT);
		barcode = GetDlgItem(IDC_BARCODE_EDIT);

		url = GetDlgItem(IDC_URL);
		
		// List view styles
		auto styles = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP;
		release_list.SetExtendedListViewStyle(styles, styles);

		// Add release list columns
		listview_helper::insert_column(release_list, artist_column, "Artist", 80);
		listview_helper::insert_column(release_list, release_column, "Release", 80);
		listview_helper::insert_column(release_list, date_column, "Date/Country", 70);
		listview_helper::insert_column(release_list, label_column, "Label/Cat#", 80);
		listview_helper::insert_column(release_list, format_column, "Format", 50);
		listview_helper::insert_column(release_list, discs_column, "Discs", 30);

		// Add release list rows
		for (t_size i = 0; i < m_release_list.size(); i++)
		{
			listview_helper::insert_item(release_list, i, m_release_list[i].album_artist, NULL);
			listview_helper::set_item_text(release_list, i, release_column, m_release_list[i].title);
			listview_helper::set_item_text(release_list, i, date_column, slasher(m_release_list[i].date, m_release_list[i].country));
			listview_helper::set_item_text(release_list, i, label_column, slasher(m_release_list[i].label, m_release_list[i].catalog));
			listview_helper::set_item_text(release_list, i, format_column, m_release_list[i].tracks[0].format);
			listview_helper::set_item_text(release_list, i, discs_column, PFC_string_formatter() << m_release_list[i].tracks[0].totaldiscs);
		}

		// Adding track list columns
		auto DPI = track_list.GetDPI();
		track_list.AddColumn("Filename", MulDiv(230, DPI.cx, 96));
		track_list.AddColumn("#", MulDiv(40, DPI.cx, 96), HDF_RIGHT);
		track_list.AddColumn("Disc Subtitle", MulDiv(120, DPI.cx, 96));
		track_list.AddColumnAutoWidth("Title");

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

		ShowWindow(SW_SHOW);
		return TRUE;
	}

	LRESULT OnLink(LPNMHDR pnmh)
	{
		ShellExecute(NULL, L"open", ((PNMLINK)pnmh)->item.szUrl, NULL, NULL, SW_SHOW);
		return 0;
	}

	LRESULT OnReleaseListChange(LPNMHDR pnmh)
	{
		int idx = ListView_GetSingleSelection(release_list);
		if (idx > -1 && idx != current_release)
		{
			current_release = idx;
			UpdateRelease();
		}
		return 0;
	}

	void OnAlbumUpdate(UINT, int, CWindow)
	{
		uGetWindowText(album, m_release_list[current_release].title);
		listview_helper::set_item_text(release_list, current_release, release_column, m_release_list[current_release].title);
	}

	void OnArtistUpdate(UINT, int, CWindow)
	{
		uGetWindowText(album_artist, m_release_list[current_release].album_artist);
		listview_helper::set_item_text(release_list, current_release, artist_column, m_release_list[current_release].album_artist);
	}

	void OnBarcodeUpdate(UINT, int, CWindow)
	{
		uGetWindowText(barcode, m_release_list[current_release].barcode);
	}

	void OnCancel(UINT, int, CWindow)
	{
		DestroyWindow();
	}

	void OnCatalogUpdate(UINT, int, CWindow)
	{
		uGetWindowText(catalog, m_release_list[current_release].catalog);
		listview_helper::set_item_text(release_list, current_release, label_column, slasher(m_release_list[current_release].label, m_release_list[current_release].catalog));
	}

	void OnClose()
	{
		DestroyWindow();
	}

	void OnDateUpdate(UINT, int, CWindow)
	{
		uGetWindowText(date, m_release_list[current_release].date);
		listview_helper::set_item_text(release_list, current_release, date_column, slasher(m_release_list[current_release].date, m_release_list[current_release].country));
	}

	void OnDiscChange(UINT, int, CWindow)
	{
		current_disc = disc.GetCurSel();
		UpdateDisc();
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

	void OnOk(UINT, int, CWindow)
	{
		tagger(m_handles, m_release_list[current_release], current_disc);
		DestroyWindow();
	}

	void OnSize(UINT nType, CSize size) {
		CRect r;
		if (!release_list.GetClientRect(r)) return;
		int w = int((r.right - r.left) * 0.2);
		release_list.SetColumnWidth(artist_column, w);
		release_list.SetColumnWidth(release_column, w);
		release_list.SetColumnWidth(label_column, w);
	}

	void OnStatusChange(UINT, int, CWindow)
	{
		m_release_list[current_release].status = get_status_str(status.GetCurSel());
	}

	void OnTypeChange(UINT, int, CWindow)
	{
		m_release_list[current_release].primary_type = get_type_str(type.GetCurSel());
	}

	void UpdateDisc()
	{
		disc.SetCurSel(current_disc);

		if (track_list.TableEdit_IsActive())
		{
			track_list.TableEdit_Abort(false);
		}

		track_list.ReloadData();
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

		auto DPI = track_list.GetDPI();
		track_list.ResizeColumn(2, m_release_list[current_release].tracks[0].totaldiscs > 1 ? MulDiv(120, DPI.cx, 96) : 0);

		if (m_release_list[current_release].is_various)
		{
			if (track_list.GetColumnCount() == 4)
			{
				track_list.AddColumn("Track Artist", MulDiv(250, DPI.cx, 96));
			}
		}
		else if (track_list.GetColumnCount() == 5)
		{
			track_list.DeleteColumn(4, false);
		}

		current_disc = 0;
		
		disc.ResetContent();

		t_size partial_lookup_matches = m_release_list[current_release].partial_lookup_matches;
		if (partial_lookup_matches > 0)
		{
			for (t_size i = 0; i < partial_lookup_matches; ++i)
			{
				disc.AddString(string_wide_from_utf8_fast(PFC_string_formatter() << "Disc " << m_release_list[current_release].tracks[i * handle_count].discnumber << " of " << m_release_list[current_release].tracks[i * handle_count].totaldiscs));
			}
		}
		else
		{
			t_size totaldiscs = m_release_list[current_release].tracks[0].totaldiscs;
			if (totaldiscs == 1)
			{
				disc.AddString(L"Showing the only disc");
			}
			else
			{
				disc.AddString(string_wide_from_utf8_fast(PFC_string_formatter() << "Showing all " << totaldiscs << " discs"));
			}
		}

		UpdateDisc();

		str8 url_str = PFC_string_formatter() << "<a href=\"" << get_server() << "/release/" << m_release_list[current_release].albumid << "\">MusicBrainz release page</a>";
		uSetWindowText(url, url_str);
	}

private:
	// IListControlOwnerDataSource methods

	bool listCanSelectItem(ctx_t, t_size) override
	{
		return false;
	}

	bool listIsColumnEditable(ctx_t, t_size sub_item) override
	{
		return sub_item > 1;
	}

	str8 listGetSubItemText(ctx_t, t_size item, t_size sub_item) override
	{
		t_size track_idx = item + (current_disc * handle_count);
		switch (sub_item)
		{
		case 0:
			return pfc::string_filename_ext(m_handles[item]->get_path());
		case 1:
			return PFC_string_formatter() << m_release_list[current_release].tracks[track_idx].discnumber << "." << m_release_list[current_release].tracks[track_idx].tracknumber;
		case 2:
			if (m_release_list[current_release].tracks[track_idx].tracknumber == 1 && m_release_list[current_release].tracks[track_idx].totaldiscs > 1)
			{
				return m_release_list[current_release].tracks[track_idx].subtitle;
			}
			return "";
		case 3:
			return m_release_list[current_release].tracks[track_idx].title;
		case 4:
			return m_release_list[current_release].tracks[track_idx].artist;
		default:
			return "";
		}
	}

	t_size listGetItemCount(ctx_t) override
	{
		return handle_count;
	}

	void listSetEditField(ctx_t, t_size item, t_size sub_item, const char* value) override
	{
		t_size track_idx = item + (current_disc * handle_count);
		switch (sub_item)
		{
		case 2:
			m_release_list[current_release].tracks[track_idx].subtitle = value;
			break;
		case 3:
			m_release_list[current_release].tracks[track_idx].title = value;
			break;
		case 4:
			m_release_list[current_release].tracks[track_idx].artist = value;
			break;
		}
	}

	void listSubItemClicked(ctx_t, t_size item, t_size sub_item) override
	{
		t_size track_idx = item + (current_disc * handle_count);
		if ((sub_item == 2 && m_release_list[current_release].tracks[track_idx].tracknumber == 1 && m_release_list[current_release].tracks[track_idx].totaldiscs > 1) || sub_item > 2)
		{
			track_list.TableEdit_Start(item, sub_item);
		}
	}

	CComboBox disc;
	CComboBox type;
	CComboBox status;
	CDialogResizeHelper m_resizer;
	CEdit album_artist;
	CEdit album;
	CEdit date;
	CEdit first_release_date;
	CEdit label;
	CEdit catalog;
	CEdit barcode;
	CListViewCtrl release_list;
	CListControlOwnerData track_list;
	CWindow url;
	metadb_handle_list m_handles;
	std::vector<Release> m_release_list;
	t_size current_release;
	t_size current_disc;
	t_size handle_count;
};
