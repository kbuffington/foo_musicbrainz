class CCustomQueryTags : public CDialogImpl<CCustomQueryTags>
{
private:
	mbCollection *mbc;
	unsigned int count;
	CButton ok;
	pfc::string8 artist;
	pfc::string8 album;

public:
	enum { IDD = IDD_CUSTOM_QUERY_TAGS };

	CCustomQueryTags(mbCollection *_mbc, unsigned int _count, pfc::string8 &_artist, pfc::string8 &_album)
		: CDialogImpl<CCustomQueryTags>(), mbc(_mbc), count(_count), artist(_artist), album(_album)
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

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		static_api_ptr_t<modeless_dialog_manager>()->add(m_hWnd);
		ok = GetDlgItem(IDOK);
		uSetDlgItemText(m_hWnd, IDC_ARTIST, artist);
		uSetDlgItemText(m_hWnd, IDC_ALBUM, album);
		return true;
	}

	void OnClose()
	{
		delete mbc;
		DestroyWindow();
	}

	void OnFinalMessage(HWND hwnd)
	{
		static_api_ptr_t<modeless_dialog_manager>()->remove(m_hWnd);
		m_hWnd = NULL;
		delete this;
	}
	
	void OnUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		if (string_utf8_from_window(m_hWnd, IDC_ARTIST).is_empty() || string_utf8_from_window(m_hWnd, IDC_ALBUM).is_empty())
		{
			ok.EnableWindow(false);
		}
		else
		{
			ok.EnableWindow(true);
		}
	}

	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		delete mbc;
		DestroyWindow();
	}

	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		pfc::string8 artist = string_utf8_from_window(m_hWnd, IDC_ARTIST);
		pfc::string8 album = string_utf8_from_window(m_hWnd, IDC_ALBUM);
		if (!artist.is_empty() && !album.is_empty())
		{
			pfc::string8 url = "ws/1/release/?type=xml&artist=";
			char tracks_count[10];
			url += URLEncode(artist);
			url += "&title=";
			url += URLEncode(album);
			url += "&count=";
			sprintf(tracks_count, "%d", count);
			url += URLEncode(tracks_count);
			new CTaggerDialog(url, mbc);
			DestroyWindow();
		}
	}
};
