#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	namespace Preferences {
		const GUID guid_server = { 0x2ac00b3b, 0x1b04, 0x4fb2,{ 0xa9, 0x98, 0x5c, 0x16, 0x4, 0x9c, 0xce, 0x9d } };
		const bool default_server = false;
		cfg_bool server(guid_server, default_server);

		const GUID guid_server_data = { 0x8732cde8, 0xe1ed, 0x4824,{ 0xae, 0xf6, 0x7d, 0x4b, 0x56, 0x99, 0x23, 0xac } };
		cfg_string server_data(guid_server_data, "https://musicbrainz.org");

		const GUID guid_short_date = { 0x18734618, 0x7920, 0x4d24,{ 0x84, 0xa1, 0xb9, 0xd6, 0x6e, 0xd8, 0x25, 0xbc } };
		const bool default_short_date = false;
		cfg_bool short_date(guid_short_date, default_short_date);

		const GUID guid_ascii_punctuation = { 0xd08b1b7c, 0x38fd, 0x4689,{ 0x9e, 0x91, 0x8c, 0xdc, 0xbe, 0xc4, 0x26, 0x98 } };
		const bool default_ascii_punctuation = false;
		cfg_bool ascii_punctuation(guid_ascii_punctuation, default_ascii_punctuation);

		const GUID guid_write_ids = { 0x8c8b61e0, 0x8eea, 0x4c34,{ 0x9a, 0x51, 0x4d, 0xa9, 0x9c, 0xec, 0xcb, 0xbc } };
		const bool default_write_ids = true;
		cfg_bool write_ids(guid_write_ids, default_write_ids);

		const GUID guid_albumtype = { 0xd8fdb1d8, 0xde2, 0x4f1f,{ 0x85, 0x5a, 0xc0, 0x5, 0x98, 0x60, 0x9b, 0xe9 } };
		const bool default_albumtype = true;
		cfg_bool albumtype(guid_albumtype, default_albumtype);

		const GUID guid_albumtype_data = { 0x20968c09, 0xb0d4, 0x4059,{ 0xb8, 0x92, 0xda, 0x76, 0xf8, 0xe6, 0x51, 0x4e } };
		cfg_string albumtype_data(guid_albumtype_data, "MUSICBRAINZ_ALBUMTYPE");

		const GUID guid_albumstatus = { 0x57a8dddd, 0xdf24, 0x4fd3,{ 0xac, 0x95, 0x6f, 0x8, 0x3, 0x26, 0x41, 0x7c } };
		const bool default_albumstatus = true;
		cfg_bool albumstatus(guid_albumstatus, default_albumstatus);

		const GUID guid_albumstatus_data = { 0x77182aac, 0x1caa, 0x4793,{ 0xb7, 0x15, 0xcc, 0xf8, 0x97, 0xba, 0x11, 0x1a } };
		cfg_string albumstatus_data(guid_albumstatus_data, "MUSICBRAINZ_ALBUMSTATUS");

		const GUID guid_write_label_info = { 0x9b3c94e3, 0x278, 0x4eb0,{ 0xa2, 0xed, 0x5, 0xad, 0xf8, 0xce, 0xa3, 0x9d } };
		const bool default_write_label_info = true;
		cfg_bool write_label_info(guid_write_label_info, default_write_label_info);

		const GUID guid_write_country = { 0xa983fbd6, 0x7471, 0x41d7,{ 0xa7, 0x62, 0x27, 0xf9, 0x94, 0xe9, 0x23, 0x1f } };
		const bool default_write_country = true;
		cfg_bool write_country(guid_write_country, default_write_country);

		const GUID guid_write_format = { 0x8e0f77e2, 0x50e8, 0x4e66,{ 0x88, 0xe3, 0xab, 0xed, 0x3b, 0x76, 0x84, 0x5a } };
		const bool default_write_format = true;
		cfg_bool write_format(guid_write_format, default_write_format);

		const GUID guid_write_albumartist = { 0x5cbeb3ba, 0xae9f, 0x4975, { 0xb9, 0x94, 0xd7, 0x50, 0xda, 0x2b, 0x22, 0x0 } };
		const bool default_write_albumartist = false;
		cfg_bool write_albumartist(guid_write_albumartist, default_write_albumartist);
	}

	class PreferencesPageInstance : public CDialogImpl<PreferencesPageInstance>, public preferences_page_instance {
	private:
		CButton server_checkbox;
		CButton short_date_checkbox;
		CButton ascii_punctuation_checkbox;
		CButton write_ids_checkbox;
		CButton write_albumtype_checkbox;
		CButton write_albumstatus_checkbox;
		CButton write_label_info_checkbox;
		CButton write_country_checkbox;
		CButton write_format_checkbox;
		CButton write_albumartist_checkbox;
		CEdit albumtype;
		CEdit albumstatus;
		CEdit server;
		preferences_page_callback::ptr on_change_callback;

	public:
		PreferencesPageInstance(preferences_page_callback::ptr callback) : on_change_callback(callback) {}

		enum { IDD = IDD_PREFERENCES };

		BEGIN_MSG_MAP(CPreferencesDialog)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_HANDLER_EX(IDC_SERVER, BN_CLICKED, OnServer)
			COMMAND_HANDLER_EX(IDC_SERVER_DATA, EN_UPDATE, OnChanged)
			COMMAND_HANDLER_EX(IDC_SHORT_DATE, BN_CLICKED, OnChanged)
			COMMAND_HANDLER_EX(IDC_ASCII_PUNCTUATION, BN_CLICKED, OnChanged)
			COMMAND_HANDLER_EX(IDC_WRITE_IDS, BN_CLICKED, OnChanged)
			COMMAND_HANDLER_EX(IDC_ALBUMTYPE, BN_CLICKED, OnAlbumType)
			COMMAND_HANDLER_EX(IDC_ALBUMTYPE_DATA, EN_UPDATE, OnChanged)
			COMMAND_HANDLER_EX(IDC_ALBUMSTATUS, BN_CLICKED, OnAlbumStatus)
			COMMAND_HANDLER_EX(IDC_ALBUMSTATUS_DATA, EN_UPDATE, OnChanged)
			COMMAND_HANDLER_EX(IDC_WRITE_LABEL_INFO, BN_CLICKED, OnChanged)
			COMMAND_HANDLER_EX(IDC_WRITE_COUNTRY, BN_CLICKED, OnChanged)
			COMMAND_HANDLER_EX(IDC_WRITE_FORMAT, BN_CLICKED, OnChanged)
			COMMAND_HANDLER_EX(IDC_WRITE_ALBUMARTIST, BN_CLICKED, OnChanged)
		END_MSG_MAP()

		BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {
			server_checkbox = GetDlgItem(IDC_SERVER);
			server = GetDlgItem(IDC_SERVER_DATA);
			short_date_checkbox = GetDlgItem(IDC_SHORT_DATE);
			ascii_punctuation_checkbox = GetDlgItem(IDC_ASCII_PUNCTUATION);
			write_ids_checkbox = GetDlgItem(IDC_WRITE_IDS);
			write_albumtype_checkbox = GetDlgItem(IDC_ALBUMTYPE);
			albumtype = GetDlgItem(IDC_ALBUMTYPE_DATA);
			write_albumstatus_checkbox = GetDlgItem(IDC_ALBUMSTATUS);
			albumstatus = GetDlgItem(IDC_ALBUMSTATUS_DATA);
			write_label_info_checkbox = GetDlgItem(IDC_WRITE_LABEL_INFO);
			write_country_checkbox = GetDlgItem(IDC_WRITE_COUNTRY);
			write_format_checkbox = GetDlgItem(IDC_WRITE_FORMAT);
			write_albumartist_checkbox = GetDlgItem(IDC_WRITE_ALBUMARTIST);

			server_checkbox.SetCheck(Preferences::server.get_value());
			short_date_checkbox.SetCheck(Preferences::short_date.get_value());
			ascii_punctuation_checkbox.SetCheck(Preferences::ascii_punctuation.get_value());
			write_ids_checkbox.SetCheck(Preferences::write_ids.get_value());
			write_albumtype_checkbox.SetCheck(Preferences::albumtype.get_value());
			write_albumstatus_checkbox.SetCheck(Preferences::albumstatus.get_value());
			write_label_info_checkbox.SetCheck(Preferences::write_label_info.get_value());
			write_country_checkbox.SetCheck(Preferences::write_country.get_value());
			write_format_checkbox.SetCheck(Preferences::write_format.get_value());
			write_albumartist_checkbox.SetCheck(Preferences::write_albumartist.get_value());

			if (Preferences::server.get_value()) server.EnableWindow(true);
			if (Preferences::albumtype.get_value()) albumtype.EnableWindow(true);
			if (Preferences::albumstatus.get_value()) albumstatus.EnableWindow(true);

			uSetWindowText(albumtype, Preferences::albumtype_data);
			uSetWindowText(albumstatus, Preferences::albumstatus_data);
			uSetWindowText(server, Preferences::server_data);

			return 0;
		}

		bool has_changed() {
			if ((bool)server_checkbox.GetCheck() != Preferences::server.get_value()) return true;
			if ((bool)short_date_checkbox.GetCheck() != Preferences::short_date.get_value()) return true;
			if ((bool)ascii_punctuation_checkbox.GetCheck() != Preferences::ascii_punctuation.get_value()) return true;
			if ((bool)write_ids_checkbox.GetCheck() != Preferences::write_ids.get_value()) return true;
			if ((bool)write_albumtype_checkbox.GetCheck() != Preferences::albumtype.get_value()) return true;
			if ((bool)write_albumstatus_checkbox.GetCheck() != Preferences::albumstatus.get_value()) return true;
			if ((bool)write_label_info_checkbox.GetCheck() != Preferences::write_label_info.get_value()) return true;
			if ((bool)write_country_checkbox.GetCheck() != Preferences::write_country.get_value()) return true;
			if ((bool)write_format_checkbox.GetCheck() != Preferences::write_format.get_value()) return true;
			if ((bool)write_albumartist_checkbox.GetCheck() != Preferences::write_albumartist.get_value()) return true;

			pfc::string8 temp;
			uGetWindowText(albumtype, temp);
			if (Preferences::albumtype_data != temp) return true;
			uGetWindowText(albumstatus, temp);
			if (Preferences::albumstatus_data != temp) return true;
			uGetWindowText(server, temp);
			if (Preferences::server_data != temp) return true;

			return false;
		}

		t_uint32 get_state() {
			t_uint32 state = preferences_state::resettable;
			if (has_changed()) state |= preferences_state::changed;
			return state;
		}

		void apply() {
			Preferences::server = (bool)server_checkbox.GetCheck();
			Preferences::short_date = (bool)short_date_checkbox.GetCheck();
			Preferences::ascii_punctuation = (bool)ascii_punctuation_checkbox.GetCheck();
			Preferences::write_ids = (bool)write_ids_checkbox.GetCheck();
			Preferences::albumtype = (bool)write_albumtype_checkbox.GetCheck();
			Preferences::albumstatus = (bool)write_albumstatus_checkbox.GetCheck();
			Preferences::write_label_info = (bool)write_label_info_checkbox.GetCheck();
			Preferences::write_country = (bool)write_country_checkbox.GetCheck();
			Preferences::write_format = (bool)write_format_checkbox.GetCheck();
			Preferences::write_albumartist = (bool)write_albumartist_checkbox.GetCheck();

			uGetWindowText(albumtype, Preferences::albumtype_data);
			uGetWindowText(albumstatus, Preferences::albumstatus_data);
			uGetWindowText(server, Preferences::server_data);
		}

		void on_change() {
			on_change_callback->on_state_changed();
		}

		void reset() {
			server_checkbox.SetCheck(Preferences::default_server);
			short_date_checkbox.SetCheck(Preferences::default_short_date);
			ascii_punctuation_checkbox.SetCheck(Preferences::default_ascii_punctuation);
			write_ids_checkbox.SetCheck(Preferences::default_write_ids);
			write_albumtype_checkbox.SetCheck(Preferences::default_albumtype);
			write_albumstatus_checkbox.SetCheck(Preferences::default_albumstatus);
			write_label_info_checkbox.SetCheck(Preferences::default_write_label_info);
			write_country_checkbox.SetCheck(Preferences::default_write_country);
			write_format_checkbox.SetCheck(Preferences::default_write_format);
			write_albumartist_checkbox.SetCheck(Preferences::default_write_albumartist);

			albumtype.EnableWindow(Preferences::default_albumtype);
			albumstatus.EnableWindow(Preferences::default_albumstatus);
			server.EnableWindow(Preferences::default_server);

			uSetWindowText(albumtype, "MUSICBRAINZ_ALBUMTYPE");
			uSetWindowText(albumstatus, "MUSICBRAINZ_ALBUMSTATUS");
			uSetWindowText(server, "https://musicbrainz.org");

			on_change();
		}

		void OnChanged(UINT, int, HWND) {
			on_change();
		}

		void OnAlbumType(UINT, int, CButton) {
			albumtype.EnableWindow((bool)write_albumtype_checkbox.GetCheck());
			on_change();
		}

		void OnAlbumStatus(UINT, int, CButton) {
			albumstatus.EnableWindow((bool)write_albumstatus_checkbox.GetCheck());
			on_change();
		}

		void OnServer(UINT, int, CButton) {
			server.EnableWindow((bool)server_checkbox.GetCheck());
			on_change();
		}
	};

	class PreferencesPage : public preferences_page_impl<PreferencesPageInstance> {
	public:
		const char * get_name() {
			return COMPONENT_TITLE;
		}

		GUID get_guid() {
			static const GUID guid = { 0x79179a37, 0x5942, 0x4fdf,{ 0xbb, 0xb7, 0x93, 0xfd, 0x35, 0xfc, 0xfe, 0x97 } };
			return guid;
		}

		GUID get_parent_guid() {
			return preferences_page::guid_tagging;
		}
	};

	preferences_page_factory_t<PreferencesPage> _;
}
