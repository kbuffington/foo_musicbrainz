#include "foo_musicbrainz.h"

static const GUID guid_cfg_short_date = { 0x18734618, 0x7920, 0x4d24, { 0x84, 0xa1, 0xb9, 0xd6, 0x6e, 0xd8, 0x25, 0xbc } };
cfg_bool cfg_short_date(guid_cfg_short_date, false);

static const GUID guid_cfg_no_feat = { 0x98f9ce46, 0xe0c5, 0x48f4, { 0xa6, 0x3e, 0x19, 0x34, 0x39, 0x64, 0x35, 0x55 } };
cfg_bool cfg_no_feat(guid_cfg_no_feat, false);

static const GUID guid_cfg_write_ids = { 0x8c8b61e0, 0x8eea, 0x4c34, { 0x9a, 0x51, 0x4d, 0xa9, 0x9c, 0xec, 0xcb, 0xbc } };
cfg_bool cfg_write_ids(guid_cfg_write_ids, true);

static const GUID guid_cfg_albumtype = { 0xd8fdb1d8, 0xde2, 0x4f1f, { 0x85, 0x5a, 0xc0, 0x5, 0x98, 0x60, 0x9b, 0xe9 } };
cfg_bool cfg_albumtype(guid_cfg_albumtype, true);

static const GUID guid_cfg_albumstatus = { 0x57a8dddd, 0xdf24, 0x4fd3, { 0xac, 0x95, 0x6f, 0x8, 0x3, 0x26, 0x41, 0x7c } };
cfg_bool cfg_albumstatus(guid_cfg_albumstatus, true);

static const GUID guid_cfg_albumtype_data = { 0x20968c09, 0xb0d4, 0x4059, { 0xb8, 0x92, 0xda, 0x76, 0xf8, 0xe6, 0x51, 0x4e } };
cfg_string cfg_albumtype_data(guid_cfg_albumtype_data, "MUSICBRAINZ_ALBUMTYPE");

static const GUID guid_cfg_albumstatus_data = { 0x77182aac, 0x1caa, 0x4793, { 0xb7, 0x15, 0xcc, 0xf8, 0x97, 0xba, 0x11, 0x1a } };
cfg_string cfg_albumstatus_data(guid_cfg_albumstatus_data, "MUSICBRAINZ_ALBUMSTATUS");

class CPreferencesDialog : public CDialogImpl<CPreferencesDialog>
{
public:
	enum { IDD = IDD_PREFERENCES };

	BEGIN_MSG_MAP(CPreferencesDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_SHORT_DATE, BN_CLICKED, OnShortDate)
		COMMAND_HANDLER_EX(IDC_NO_FEAT, BN_CLICKED, OnNoFeat)
		COMMAND_HANDLER_EX(IDC_WRITE_IDS, BN_CLICKED, OnWriteIDs)
		COMMAND_HANDLER_EX(IDC_ALBUMTYPE, BN_CLICKED, OnAlbumType)
		COMMAND_HANDLER_EX(IDC_ALBUMSTATUS, BN_CLICKED, OnAlbumStatus)
		COMMAND_HANDLER_EX(IDC_ALBUMTYPE_DATA, EN_UPDATE, OnAlbumTypeUpdate)
		COMMAND_HANDLER_EX(IDC_ALBUMSTATUS_DATA, EN_UPDATE, OnAlbumStatusUpdate)
	END_MSG_MAP()

	void OnFinalMessage(HWND hwnd)
	{
		delete this;
	}

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		CEdit cfg_edit_albumtype_data = GetDlgItem(IDC_ALBUMTYPE_DATA);
		CEdit cfg_edit_albumstatus_data = GetDlgItem(IDC_ALBUMSTATUS_DATA);
		((CButton)GetDlgItem(IDC_SHORT_DATE)).SetCheck(cfg_short_date.get_value());
		((CButton)GetDlgItem(IDC_NO_FEAT)).SetCheck(cfg_no_feat.get_value());
		((CButton)GetDlgItem(IDC_WRITE_IDS)).SetCheck(cfg_write_ids.get_value());
		((CButton)GetDlgItem(IDC_ALBUMTYPE)).SetCheck(cfg_albumtype.get_value());
		((CButton)GetDlgItem(IDC_ALBUMSTATUS)).SetCheck(cfg_albumstatus.get_value());
		uSetWindowText(cfg_edit_albumtype_data, cfg_albumtype_data);
		uSetWindowText(cfg_edit_albumstatus_data, cfg_albumstatus_data);
		if (cfg_albumtype.get_value()) cfg_edit_albumtype_data.EnableWindow(1);
		if (cfg_albumstatus.get_value()) cfg_edit_albumstatus_data.EnableWindow(1);
		return 0;
	}

	void OnShortDate(UINT uNotifyCode, int nID, CButton wndCtl)
	{
		cfg_short_date = (bool)wndCtl.GetCheck();
	}

	void OnNoFeat(UINT uNotifyCode, int nID, CButton wndCtl)
	{
		cfg_no_feat = (bool)wndCtl.GetCheck();
	}

	void OnWriteIDs(UINT uNotifyCode, int nID, CButton wndCtl)
	{
		cfg_write_ids = (bool)wndCtl.GetCheck();
	}

	void OnAlbumType(UINT uNotifyCode, int nID, CButton wndCtl)
	{
		cfg_albumtype = (bool)wndCtl.GetCheck();
		GetDlgItem(IDC_ALBUMTYPE_DATA).EnableWindow(cfg_albumtype.get_value());
	}

	void OnAlbumStatus(UINT uNotifyCode, int nID, CButton wndCtl)
	{
		cfg_albumstatus = (bool)wndCtl.GetCheck();
		GetDlgItem(IDC_ALBUMSTATUS_DATA).EnableWindow(cfg_albumstatus.get_value());
	}

	void OnAlbumTypeUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		uGetWindowText(wndCtl, cfg_albumtype_data);
	}

	void OnAlbumStatusUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		uGetWindowText(wndCtl, cfg_albumstatus_data);
	}
};

class foo_mb_preferences_page : public preferences_page_v2
{
public:
	HWND create(HWND parent)
	{
		CPreferencesDialog *PreferencesDialog = new CPreferencesDialog();
		PreferencesDialog->Create(parent);
		return PreferencesDialog->m_hWnd;
	}

	const char * get_name()
	{
		return "MusicBrainz Tagger";
	}

	GUID get_guid()
	{
		static const GUID guid_foo_mb_preferences_page = { 0x79179a37, 0x5942, 0x4fdf, { 0xbb, 0xb7, 0x93, 0xfd, 0x35, 0xfc, 0xfe, 0x97 } };
		return guid_foo_mb_preferences_page;
	}

	GUID get_parent_guid()
	{
		return preferences_page::guid_tagging;
	}

	bool reset_query()
	{
		return true;
	}

	void reset()
	{
		cfg_short_date = false;
		cfg_no_feat = false;
		cfg_write_ids = true;
		cfg_albumtype = true;
		cfg_albumstatus = true;
		cfg_albumtype_data = "MUSICBRAINZ_ALBUMTYPE";
		cfg_albumstatus_data = "MUSICBRAINZ_ALBUMSTATUS";
	}

	double get_sort_priority()
	{
		return 0;
	}
};

static preferences_page_factory_t<foo_mb_preferences_page> g_foo_mb_preferences_page_factory;
