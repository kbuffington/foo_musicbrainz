#include <regex>

using namespace std::tr1;

class CCustomQueryMBID : public CDialogImpl<CCustomQueryMBID>
{
private:
	mbCollection *mbc;
	unsigned int count;
	CButton ok;
	pfc::string8 mbid;

public:
	enum { IDD = IDD_CUSTOM_QUERY_MBID };

	CCustomQueryMBID(mbCollection *_mbc, unsigned int _count, pfc::string8 &_mbid) : CDialogImpl<CCustomQueryMBID>(), mbc(_mbc), count(_count), mbid(_mbid)
	{
		Create(core_api::get_main_window());
	}

	BEGIN_MSG_MAP(CCustomQueryMBID)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_HANDLER_EX(IDC_MBID, EN_UPDATE, OnUpdate)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		static_api_ptr_t<modeless_dialog_manager>()->add(m_hWnd);
		ok = GetDlgItem(IDOK);
		uSetDlgItemText(m_hWnd, IDC_MBID, mbid);
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
		regex rx("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$");
		if (regex_search(string_utf8_from_window(m_hWnd, IDC_MBID).get_ptr(), rx))
		{
			ok.EnableWindow(true);
		}
		else ok.EnableWindow(false);
	}

	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		delete mbc;
		DestroyWindow();
	}

	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		pfc::string8 url = "ws/1/release/";
		url += URLEncode(string_utf8_from_window(m_hWnd, IDC_MBID));
		url += "?type=xml&inc=artist+release-events+tracks";
		new CTaggerDialog(url, mbc);
		DestroyWindow();
	}
};
