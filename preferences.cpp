#include "foo_musicbrainz.h"

static const GUID guid_cfg_short_date = { 0x18734618, 0x7920, 0x4d24, { 0x84, 0xa1, 0xb9, 0xd6, 0x6e, 0xd8, 0x25, 0xbc } };
cfg_bool cfg_short_date(guid_cfg_short_date, false);

static const GUID guid_cfg_write_ids = { 0x8c8b61e0, 0x8eea, 0x4c34, { 0x9a, 0x51, 0x4d, 0xa9, 0x9c, 0xec, 0xcb, 0xbc } };
cfg_bool cfg_write_ids(guid_cfg_write_ids, true);

static const GUID guid_cfg_no_feat = { 0x98f9ce46, 0xe0c5, 0x48f4, { 0xa6, 0x3e, 0x19, 0x34, 0x39, 0x64, 0x35, 0x55 } };
cfg_bool cfg_no_feat(guid_cfg_no_feat, false);

class CPreferencesDialog : public CDialogImpl<CPreferencesDialog>
{
private:
	CButton cfg_check_short_date;
	CButton cfg_check_write_ids;
	CButton cfg_check_no_feat;

public:
	enum { IDD = IDD_PREFERENCES };

	BEGIN_MSG_MAP(CPreferencesDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_SHORT_DATE, BN_CLICKED, OnShortDate)
		COMMAND_HANDLER_EX(IDC_WRITE_IDS, BN_CLICKED, OnWriteIDs)
		COMMAND_HANDLER_EX(IDC_NO_FEAT, BN_CLICKED, OnNoFeat)
	END_MSG_MAP()

	void OnFinalMessage(HWND hwnd)
	{
		m_hWnd = NULL;
		delete this;
	}

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		cfg_check_short_date = GetDlgItem(IDC_SHORT_DATE);
		cfg_check_write_ids = GetDlgItem(IDC_WRITE_IDS);
		cfg_check_no_feat = GetDlgItem(IDC_NO_FEAT);
		cfg_check_short_date.SetCheck(cfg_short_date.get_value());
		cfg_check_write_ids.SetCheck(cfg_write_ids.get_value());
		cfg_check_no_feat.SetCheck(cfg_no_feat.get_value());
		return 0;
	}

	void OnShortDate(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		cfg_short_date = (bool)cfg_check_short_date.GetCheck();
	}

	void OnWriteIDs(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		cfg_write_ids = (bool)cfg_check_write_ids.GetCheck();
	}

	void OnNoFeat(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		cfg_no_feat = (bool)cfg_check_no_feat.GetCheck();
	}
};

class foo_mb_preferences_page: public preferences_page_v2
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
		cfg_write_ids = true;
		cfg_no_feat = false;
    }

	double get_sort_priority()
	{
		return 0;
	}

};

static preferences_page_factory_t<foo_mb_preferences_page> g_foo_mb_preferences_page_factory;
