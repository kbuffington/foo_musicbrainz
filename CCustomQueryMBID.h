#include <regex>
#include "Query.h"

using namespace std::tr1;
using namespace foo_musicbrainz;

class CCustomQueryMBID : public CDialogImpl<CCustomQueryMBID>
{
private:
	ReleaseList *mbc;
	unsigned int count;
	CButton ok;
	pfc::string8 mbid;

public:
	enum { IDD = IDD_CUSTOM_QUERY_MBID };

	CCustomQueryMBID(ReleaseList *_mbc, unsigned int _count, pfc::string8 &_mbid) : CDialogImpl<CCustomQueryMBID>(), mbc(_mbc), count(_count), mbid(_mbid)
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
		auto query = new foo_musicbrainz::Query(string_utf8_from_window(m_hWnd, IDC_MBID).get_ptr());
		new CTaggerDialog(query, mbc);
		DestroyWindow();
	}
};
