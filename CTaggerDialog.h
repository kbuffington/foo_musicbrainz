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
	enum { IDD = IDD_TAGGER_DIALOG };

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
		NOTIFY_HANDLER_EX(IDC_URL, NM_RETURN, OnLink)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_HANDLER_EX(IDC_TYPE, CBN_SELENDOK, OnTypeChange)
		COMMAND_HANDLER_EX(IDC_STATUS, CBN_SELENDOK, OnStatusChange)
		//CHAIN_MSG_MAP(CDialogImpl<CTaggerDialog>)
	END_MSG_MAP()

	bool OnInitDialog(HWND wParam, LPARAM lParam)
	{
		static_api_ptr_t<modeless_dialog_manager>()->add(m_hWnd);
		release_list = GetDlgItem(IDC_RELEASE_LIST);
		track_list = GetDlgItem(IDC_TRACK_LIST);
		type = GetDlgItem(IDC_TYPE);
		status = GetDlgItem(IDC_STATUS);
		url = GetDlgItem(IDC_URL);
		
		LVCOLUMN column_item;
		uconvert str;

		track_list_view = new track_list_view_edit(track_list, mbc);
		
		// List view styles
		ListView_SetExtendedListViewStyleEx(release_list, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
		ListView_SetExtendedListViewStyleEx(track_list, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);

		// Adding release list columns
		column_item.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		wchar_t *release_list_columns[] = { L"Artist", L"Release", L"Date" };
		int col_size[] = { 180, 180, 70 };
		for (int i = 0; i < 3; i++) 
		{ 
			column_item.iSubItem = i;
			column_item.pszText = release_list_columns[i];
			column_item.cx = col_size[i];
			ListView_InsertColumn(release_list, i, &column_item);
		}

		// Adding track list columns
		// Fake column
		column_item.mask = 0;
		ListView_InsertColumn(track_list, 0, &column_item);

		column_item.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		column_item.fmt = LVCFMT_RIGHT;
		column_item.cx = 30;
		column_item.pszText = L"#";
		ListView_InsertColumn(track_list, 1, &column_item);

		column_item.mask = LVCF_WIDTH | LVCF_TEXT;
		column_item.cx = 390;
		column_item.pszText = L"Title";
		ListView_InsertColumn(track_list, 2, &column_item);

		ListView_DeleteColumn(track_list, 0);

		for (int i = 0; i < MB_RELEASE_TYPES; i++)
		{
			SendMessage(type, CB_ADDSTRING, 0, (LPARAM)str.ToUtf16(mbRelease::Types[i]));
		}
		for (int i = 0; i < MB_RELEASE_STATUSES; i++)
		{
			SendMessage(status, CB_ADDSTRING, 0, (LPARAM)str.ToUtf16(mbRelease::Statuses[i]));
		}
		return true;
	}

	LRESULT OnUpdateReleaseList(UINT Message, WPARAM wParam, LPARAM lParam, BOOL bHandled)
	{
		LVITEM list_item;
		uconvert str;

		list_item.mask = LVIF_TEXT;
		list_item.iSubItem = 0;

		ListView_DeleteAllItems(release_list);
		for (list_item.iItem = 0; list_item.iItem < (int)mbc->getReleasesCount(); list_item.iItem++)
		{
			list_item.pszText = str.ToUtf16(mbc->getRelease(list_item.iItem)->getArtist());
			ListView_InsertItem(release_list, &list_item);
			ListView_SetItemText(release_list, list_item.iItem, 1, str.ToUtf16(mbc->getRelease(list_item.iItem)->getTitle()));
			ListView_SetItemText(release_list, list_item.iItem, 2, str.ToUtf16(mbc->getRelease(list_item.iItem)->getDate()));
		}

		mbRelease *release = mbc->getRelease();
		wchar_t track_number_str[10];
		HWND track_list = GetDlgItem(IDC_TRACK_LIST);

		list_item.mask = LVIF_TEXT;
		list_item.iSubItem = 0;

		for (list_item.iItem = 0; list_item.iItem < (int)release->getTracksCount(); list_item.iItem++)
		{
			//column 1 - track_number
			wsprintf(track_number_str, L"%u", list_item.iItem+1);
			list_item.pszText = track_number_str;
			ListView_InsertItem(track_list, &list_item);
		}
		SendMessage(m_hWnd, WM_FOO_MB_UPDATE_RELEASE, 0, 0);
		return 0;
	}

	LRESULT OnUpdateRelease(UINT Message, WPARAM wParam, LPARAM lParam, BOOL bHandled)
	{
		mbRelease *release = mbc->getRelease();
		uconvert str;

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
			ListView_DeleteColumn(track_list, 2);
			ListView_SetColumnWidth(track_list, 1, 390);
		}
		else if (!ListView_GetColumnWidth(track_list, 2) && release->va)
		{
			LVCOLUMN column_item;
			column_item.mask = LVCF_WIDTH | LVCF_TEXT;
			column_item.pszText = L"Track artist";
			column_item.cx = 195;
			ListView_InsertColumn(track_list, 2, &column_item);
			ListView_SetColumnWidth(track_list, 1, 195);
		}

		// Tracks
		for (int iItem = 0; iItem < (int)release->getTracksCount(); iItem++)
		{
			//column 2 - title
			ListView_SetItemText(track_list, iItem, 1, str.ToUtf16(release->getTrack(iItem)->getTitle()));
			ListView_SetItemText(track_list, iItem, 2, str.ToUtf16(release->getTrack(iItem)->getArtist()));
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
			track_list_view->start(((LPNMITEMACTIVATE)pnmh)->iItem, ((LPNMITEMACTIVATE)pnmh)->iSubItem);
		}
		return 0;
	}

	LRESULT OnReleaseListChange(LPNMHDR pnmh)
	{
		if (((LPNMLISTVIEW)pnmh)->iItem != -1 && ((LPNMLISTVIEW)pnmh)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)pnmh)->uNewState & LVIS_SELECTED)
		{
			if (mbc->getCurrentRelease() != ((LPNMITEMACTIVATE)pnmh)->iItem)
			{
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
};
