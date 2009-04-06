class CTaggerDialog : public CDialogImpl<CTaggerDialog>
{
private:
	HWND release_list;
	HWND track_list;
	HWND type;
	HWND status;
	HWND url;
	mbCollection *mbc;
	track_list_view_edit *track_list_view;

public:
	enum { IDD = IDD_TAGGER };

	CTaggerDialog(pfc::string8 url, mbCollection *_mbc) : CDialogImpl<CTaggerDialog>()
	{
		mbc = _mbc;
		Create(core_api::get_main_window());
		threaded_process::g_run_modeless(new service_impl_t<foo_mb_request_thread>(url, m_hWnd, mbc), threaded_process::flag_show_progress | threaded_process::flag_show_abort, m_hWnd, "Quering information from MusicBrainz");
	}

	BEGIN_MSG_MAP(CTaggerDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER(WM_FOO_MB_UPDATE_RELEASES_LIST, OnUpdateReleaseList)
		MESSAGE_HANDLER(WM_FOO_MB_UPDATE_RELEASE, OnUpdateRelease)
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
		COMMAND_HANDLER_EX(IDC_DISC, EN_UPDATE, OnDiscUpdate)
		COMMAND_HANDLER_EX(IDC_DISCSUBTITLE, EN_UPDATE, OnDiscSubtitleUpdate)
		//CHAIN_MSG_MAP(CDialogImpl<CTaggerDialog>)
	END_MSG_MAP()

	bool OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		static_api_ptr_t<modeless_dialog_manager>()->add(m_hWnd);
		release_list = GetDlgItem(IDC_RELEASE_LIST);
		track_list = GetDlgItem(IDC_TRACK_LIST);
		type = GetDlgItem(IDC_TYPE);
		status = GetDlgItem(IDC_STATUS);
		url = GetDlgItem(IDC_URL);
		
		LVCOLUMN column_item;

		track_list_view = new track_list_view_edit(track_list, mbc);
		
		// List view styles
		ListView_SetExtendedListViewStyleEx(release_list, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
		ListView_SetExtendedListViewStyleEx(track_list, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);

		// Adding release list columns
		listview_helper::insert_column(release_list, 0, "Artist", 115);
		listview_helper::insert_column(release_list, 1, "Release", 115);
		listview_helper::insert_column(release_list, 2, "Date", 49);

		// Adding track list columns
		// Fake column
		listview_helper::insert_column(track_list, 0, "", 0);

		column_item.mask = 0;
		ListView_InsertColumn(track_list, 0, &column_item);

		column_item.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		column_item.fmt = LVCFMT_RIGHT;
		column_item.cx = 30;
		column_item.pszText = L"#";
		ListView_InsertColumn(track_list, 1, &column_item);

		listview_helper::insert_column(track_list, 2, "Title", 260);

		ListView_DeleteColumn(track_list, 0);

		for (int i = 0; i < MB_RELEASE_TYPES; i++)
		{
			pfc::stringcvt::string_os_from_utf8 str(mbRelease::Types[i]);
			SendMessage(type, CB_ADDSTRING, 0, (LPARAM)str.get_ptr());
		}
		for (int i = 0; i < MB_RELEASE_STATUSES; i++)
		{
			pfc::stringcvt::string_os_from_utf8 str(mbRelease::Statuses[i]);
			SendMessage(status, CB_ADDSTRING, 0, (LPARAM)str.get_ptr());
		}
		return true;
	}

	LRESULT OnUpdateReleaseList(UINT Message, WPARAM wParam, LPARAM lParam, BOOL bHandled)
	{
		for (unsigned int i = 0; i < mbc->getReleasesCount(); i++)
		{
			listview_helper::insert_item(release_list, i, mbc->getRelease(i)->getArtist(), NULL);
			listview_helper::set_item_text(release_list, i, 1, mbc->getRelease(i)->getTitle());
			listview_helper::set_item_text(release_list, i, 2, mbc->getRelease(i)->getDate());
		}

		mbRelease *release = mbc->getRelease();
		char track_number_str[10];

		for (unsigned int i = 0; i < release->getTracksCount(); i++)
		{
			sprintf(track_number_str, "%u", i+1);
			listview_helper::insert_item(track_list, i, track_number_str, NULL);
		}
		SendMessage(WM_FOO_MB_UPDATE_RELEASE, 0, 0);
		return 0;
	}

	LRESULT OnUpdateRelease(UINT Message, WPARAM wParam, LPARAM lParam, BOOL bHandled)
	{
		mbRelease *release = mbc->getRelease();

		uSetDlgItemText(m_hWnd, IDC_ARTIST, release->getArtist());
		uSetDlgItemText(m_hWnd, IDC_ALBUM, release->getTitle());
		uSetDlgItemText(m_hWnd, IDC_DATE, release->getDate());
		uSetDlgItemText(m_hWnd, IDC_DISC, release->getDisc());
		uSetDlgItemText(m_hWnd, IDC_DISCSUBTITLE, release->getDiscSubtitle());

		SendMessage(GetDlgItem(IDC_TYPE), CB_SETCURSEL, (WPARAM)mbc->getRelease()->getType(), 0);
		SendMessage(GetDlgItem(IDC_STATUS), CB_SETCURSEL, (WPARAM)mbc->getRelease()->getStatus(), 0);

		// VA?
		
		if (ListView_GetColumnWidth(track_list, 2) && !release->va)
		{
			ListView_SetColumnWidth(track_list, 1, 260);
			ListView_DeleteColumn(track_list, 2);
		}
		else if (!ListView_GetColumnWidth(track_list, 2) && release->va)
		{
			ListView_SetColumnWidth(track_list, 1, 195);
			listview_helper::insert_column(track_list, 2, "Track artist", 130);
		}

		// Tracks
		for (int iItem = 0; iItem < (int)release->getTracksCount(); iItem++)
		{
			listview_helper::set_item_text(track_list, iItem, 1, release->getTrack(iItem)->getTitle());
			listview_helper::set_item_text(track_list, iItem, 2, release->getTrack(iItem)->getArtist());
		}

		// Link
		pfc::string8 url;
		url = "<a href=\"http://musicbrainz.org/release/";
		url += release->getId();
		url += ".html\">MusicBrainz release page</a>";
		uSetDlgItemText(m_hWnd, IDC_URL, url);
		return 0;
	}

	LRESULT OnTrackListClick(LPNMHDR pnmh)
	{
		if (((LPNMITEMACTIVATE)pnmh)->iItem != -1 && ((LPNMITEMACTIVATE)pnmh)->iSubItem != 0)
		{
			track_list_view->Start(((LPNMITEMACTIVATE)pnmh)->iItem, ((LPNMITEMACTIVATE)pnmh)->iSubItem);
		}
		return 0;
	}

	LRESULT OnReleaseListChange(LPNMHDR pnmh)
	{
		if (((LPNMLISTVIEW)pnmh)->iItem != -1 && ((LPNMLISTVIEW)pnmh)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)pnmh)->uNewState & LVIS_SELECTED)
		{
			if (mbc->getCurrentRelease() != ((LPNMITEMACTIVATE)pnmh)->iItem)
			{
				if (track_list_view->IsActive()) track_list_view->Abort();
				mbc->setCurrentRelease(((LPNMITEMACTIVATE)pnmh)->iItem);
				SendMessage(WM_FOO_MB_UPDATE_RELEASE, 0, 0);
			}
		}
		return 0;
	}

	LRESULT OnLink(LPNMHDR pnmh)
	{
		ShellExecute(NULL, L"open", ((PNMLINK)pnmh)->item.szUrl, NULL, NULL, SW_SHOW);
		return 0;
	}

	void OnClose()
	{
		DestroyWindow();
	}

	void OnFinalMessage(HWND hwnd)
	{
		static_api_ptr_t<modeless_dialog_manager>()->remove(m_hWnd);
		delete track_list_view;
		delete mbc;
		m_hWnd = NULL;
		delete this;
	}

	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		DestroyWindow();
	}

	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		ShowWindow(SW_HIDE);
		static_api_ptr_t<metadb_io_v2>()->update_info_async(*mbc->getData(),new service_impl_t<foo_mb_file_info_filter_impl>(m_hWnd, mbc),core_api::get_main_window(), metadb_io_v2::op_flag_delay_ui, NULL);
	}

	void OnTypeChange(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		mbc->getRelease()->setType(SendMessage(wndCtl, CB_GETCURSEL, 0,0));
	}

	void OnStatusChange(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		mbc->getRelease()->setStatus(SendMessage(wndCtl, CB_GETCURSEL, 0,0));
	}

	void OnArtistUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		pfc::string8 str;
		uGetDlgItemText(m_hWnd, nID, str);
		mbc->getRelease()->setArtist(str);
		listview_helper::set_item_text(release_list, mbc->getCurrentRelease(), 0, str);
	}

	void OnAlbumUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		pfc::string8 str;
		uGetDlgItemText(m_hWnd, nID, str);
		mbc->getRelease()->setTitle(str);
		listview_helper::set_item_text(release_list, mbc->getCurrentRelease(), 1, str);
	}

	void OnDateUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		pfc::string8 str;
		uGetDlgItemText(m_hWnd, nID, str);
		mbc->getRelease()->setDate(str);
		listview_helper::set_item_text(release_list, mbc->getCurrentRelease(), 2, str);
	}

	void OnDiscUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		mbc->getRelease()->setDisc(uGetDlgItemText(m_hWnd, nID).get_ptr());
	}

	void OnDiscSubtitleUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		mbc->getRelease()->setDiscSubtitle(uGetDlgItemText(m_hWnd, nID).get_ptr());
	}
};
