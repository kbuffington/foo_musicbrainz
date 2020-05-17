#include "stdafx.h"

namespace prefs
{
	namespace guids
	{
		static constexpr GUID check_server = { 0x2ac00b3b, 0x1b04, 0x4fb2,{ 0xa9, 0x98, 0x5c, 0x16, 0x4, 0x9c, 0xce, 0x9d } };
		static constexpr GUID check_short_date = { 0x18734618, 0x7920, 0x4d24,{ 0x84, 0xa1, 0xb9, 0xd6, 0x6e, 0xd8, 0x25, 0xbc } };
		static constexpr GUID check_ascii_punctuation = { 0xd08b1b7c, 0x38fd, 0x4689,{ 0x9e, 0x91, 0x8c, 0xdc, 0xbe, 0xc4, 0x26, 0x98 } };
		static constexpr GUID check_write_ids = { 0x8c8b61e0, 0x8eea, 0x4c34,{ 0x9a, 0x51, 0x4d, 0xa9, 0x9c, 0xec, 0xcb, 0xbc } };
		static constexpr GUID check_write_albumid = { 0xdbdbe746, 0x71bf, 0x4e44, { 0x8b, 0x74, 0xb4, 0xf8, 0xb3, 0x5a, 0x5e, 0xe9 } };
		static constexpr GUID check_write_discids = { 0x631b7da4, 0xab6b, 0x4be3, { 0xad, 0x9, 0x47, 0x7c, 0x8e, 0x3, 0xd6, 0x57 } };
		static constexpr GUID check_write_trackids = { 0xc406c2a7, 0x5a72, 0x4bbf, { 0x8d, 0xb9, 0x59, 0x88, 0x1e, 0x41, 0xe4, 0xff } };
		static constexpr GUID check_write_albumtype = { 0xd8fdb1d8, 0xde2, 0x4f1f,{ 0x85, 0x5a, 0xc0, 0x5, 0x98, 0x60, 0x9b, 0xe9 } };
		static constexpr GUID check_write_albumstatus = { 0x57a8dddd, 0xdf24, 0x4fd3,{ 0xac, 0x95, 0x6f, 0x8, 0x3, 0x26, 0x41, 0x7c } };
		static constexpr GUID check_write_label_info = { 0x9b3c94e3, 0x278, 0x4eb0, { 0xa2, 0xed, 0x5, 0xad, 0xf8, 0xce, 0xa3, 0x9d } };
		static constexpr GUID check_write_country = { 0xa983fbd6, 0x7471, 0x41d7, { 0xa7, 0x62, 0x27, 0xf9, 0x94, 0xe9, 0x23, 0x1f } };
		static constexpr GUID check_write_media = { 0x8e0f77e2, 0x50e8, 0x4e66,{ 0x88, 0xe3, 0xab, 0xed, 0x3b, 0x76, 0x84, 0x5a } };
		static constexpr GUID check_write_asin = { 0x6677d957, 0xaa52, 0x4fef, { 0x95, 0x81, 0xb3, 0xe3, 0x20, 0x16, 0xaf, 0x4d } };
		static constexpr GUID check_write_isrc = { 0x8b129571, 0x1bf2, 0x41bd, { 0x86, 0xf1, 0xcd, 0xec, 0x6, 0xb9, 0xf3, 0xff } };
		static constexpr GUID check_write_albumartist = { 0x5cbeb3ba, 0xae9f, 0x4975, { 0xb9, 0x94, 0xd7, 0x50, 0xda, 0x2b, 0x22, 0x0 } };

		static constexpr GUID str_server = { 0x8732cde8, 0xe1ed, 0x4824,{ 0xae, 0xf6, 0x7d, 0x4b, 0x56, 0x99, 0x23, 0xac } };
		static constexpr GUID str_albumtype = { 0x20968c09, 0xb0d4, 0x4059,{ 0xb8, 0x92, 0xda, 0x76, 0xf8, 0xe6, 0x51, 0x4e } };
		static constexpr GUID str_albumstatus = { 0x77182aac, 0x1caa, 0x4793,{ 0xb7, 0x15, 0xcc, 0xf8, 0x97, 0xba, 0x11, 0x1a } };
	}

	namespace defaults
	{
		static constexpr bool check_server = false;
		static constexpr bool check_short_date = false;
		static constexpr bool check_ascii_punctuation = false;
		static constexpr bool check_write_ids = true;
		static constexpr bool check_write_albumid = true;
		static constexpr bool check_write_discids = true;
		static constexpr bool check_write_trackids = true;
		static constexpr bool check_write_albumtype = true;
		static constexpr bool check_write_albumstatus = true;
		static constexpr bool check_write_label_info = true;
		static constexpr bool check_write_country = true;
		static constexpr bool check_write_media = true;
		static constexpr bool check_write_asin = true;
		static constexpr bool check_write_isrc = true;
		static constexpr bool check_write_albumartist = false;

		static constexpr const char* str_server = "https://musicbrainz.org";
		static constexpr const char* str_albumtype = "MUSICBRAINZ_ALBUMTYPE";
		static constexpr const char* str_albumstatus = "MUSICBRAINZ_ALBUMSTATUS";
	}

	namespace check
	{
		cfg_bool server(guids::check_server, defaults::check_server);
		cfg_bool short_date(guids::check_short_date, defaults::check_short_date);
		cfg_bool ascii_punctuation(guids::check_ascii_punctuation, defaults::check_ascii_punctuation);
		cfg_bool write_ids(guids::check_write_ids, defaults::check_write_ids);
		cfg_bool write_albumid(guids::check_write_albumid, defaults::check_write_albumid);
		cfg_bool write_discids(guids::check_write_discids, defaults::check_write_discids);
		cfg_bool write_trackids(guids::check_write_trackids, defaults::check_write_trackids);
		cfg_bool write_albumtype(guids::check_write_albumtype, defaults::check_write_albumtype);
		cfg_bool write_albumstatus(guids::check_write_albumstatus, defaults::check_write_albumstatus);
		cfg_bool write_label_info(guids::check_write_label_info, defaults::check_write_label_info);
		cfg_bool write_country(guids::check_write_country, defaults::check_write_country);
		cfg_bool write_media(guids::check_write_media, defaults::check_write_media);
		cfg_bool write_asin(guids::check_write_asin, defaults::check_write_asin);
		cfg_bool write_isrc(guids::check_write_isrc, defaults::check_write_isrc);
		cfg_bool write_albumartist(guids::check_write_albumartist, defaults::check_write_albumartist);
	}

	namespace str
	{
		cfg_string server(guids::str_server, defaults::str_server);
		cfg_string albumtype(guids::str_albumtype, defaults::str_albumtype);
		cfg_string albumstatus(guids::str_albumstatus, defaults::str_albumstatus);
	}

	str8 get_server()
	{
		return check::server ? str::server.get_ptr() : defaults::str_server;
	}
}

namespace mb
{
	class my_preferences_page_instance : public CDialogImpl<my_preferences_page_instance>, public preferences_page_instance
	{
	public:
		my_preferences_page_instance(preferences_page_callback::ptr callback) : m_callback(callback) {}

		BEGIN_MSG_MAP_EX(my_preferences_page_instance)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_CODE_HANDLER_EX(EN_UPDATE, OnChanged)
			COMMAND_RANGE_HANDLER_EX(IDC_CHECK_SERVER, IDC_CHECK_WRITE_ALBUMARTIST, OnChanged)
		END_MSG_MAP()

		enum { IDD = IDD_PREFERENCES };

		BOOL OnInitDialog(CWindow, LPARAM)
		{
			using namespace prefs;

			m_check_map[IDC_CHECK_SERVER] = { &check::server, defaults::check_server };
			m_check_map[IDC_CHECK_SHORT_DATE] = { &check::short_date, defaults::check_short_date };
			m_check_map[IDC_CHECK_ASCII_PUNCTUATION] = { &check::ascii_punctuation, defaults::check_ascii_punctuation };
			m_check_map[IDC_CHECK_WRITE_IDS] = { &check::write_ids, defaults::check_write_ids };
			m_check_map[IDC_CHECK_WRITE_ALBUMID] = { &check::write_albumid, defaults::check_write_albumid, IDC_CHECK_WRITE_IDS };
			m_check_map[IDC_CHECK_WRITE_DISCIDS] = { &check::write_discids, defaults::check_write_discids, IDC_CHECK_WRITE_IDS };
			m_check_map[IDC_CHECK_WRITE_TRACKIDS] = { &check::write_trackids, defaults::check_write_trackids, IDC_CHECK_WRITE_IDS };
			m_check_map[IDC_CHECK_WRITE_ALBUMTYPE] = { &check::write_albumtype, defaults::check_write_albumtype };
			m_check_map[IDC_CHECK_WRITE_ALBUMSTATUS] = { &check::write_albumstatus, defaults::check_write_albumstatus };
			m_check_map[IDC_CHECK_WRITE_LABEL_INFO] = { &check::write_label_info, defaults::check_write_label_info };
			m_check_map[IDC_CHECK_WRITE_COUNTRY] = { &check::write_country, defaults::check_write_country };
			m_check_map[IDC_CHECK_WRITE_MEDIA] = { &check::write_media, defaults::check_write_media };
			m_check_map[IDC_CHECK_WRITE_ASIN] = { &check::write_asin, defaults::check_write_asin };
			m_check_map[IDC_CHECK_WRITE_ISRC] = { &check::write_isrc, defaults::check_write_isrc };
			m_check_map[IDC_CHECK_WRITE_ALBUMARTIST] = { &check::write_albumartist, defaults::check_write_albumartist };

			for (auto& [key, value] : m_check_map)
			{
				value.check = GetDlgItem(key);
				if (value.id) {
					value.check.EnableWindow(m_check_map.at(value.id).setting->get_value());
				}
				value.check.SetCheck(value.setting->get_value());
			}

			m_str_map[IDC_EDIT_SERVER] = { &str::server, defaults::str_server, IDC_CHECK_SERVER };
			m_str_map[IDC_EDIT_ALBUMTYPE] = { &str::albumtype, defaults::str_albumtype, IDC_CHECK_WRITE_ALBUMTYPE };
			m_str_map[IDC_EDIT_ALBUMSTATUS] = { &str::albumstatus, defaults::str_albumstatus, IDC_CHECK_WRITE_ALBUMSTATUS };

			for (auto& [key, value] : m_str_map)
			{
				value.edit = GetDlgItem(key);
				value.edit.EnableWindow(m_check_map.at(value.id).setting->get_value());
				uSetWindowText(value.edit, value.setting->get_ptr());
			}

			return FALSE;
		}

		bool has_changed()
		{
			for (auto& [key, value] : m_check_map)
			{
				if (value.check.IsChecked() != value.setting->get_value()) return true;
			}

			for (auto& [key, value] : m_str_map)
			{
				str8 temp;
				uGetWindowText(value.edit, temp);
				if (!temp.equals(value.setting->get_ptr())) return true;
			}

			return false;
		}

		t_uint32 get_state() override
		{
			t_uint32 state = preferences_state::resettable;
			if (has_changed()) state |= preferences_state::changed;
			return state;
		}

		void apply() override
		{
			for (auto& [key, value] : m_check_map)
			{
				*value.setting = value.check.IsChecked();
			}

			for (auto& [key, value] : m_str_map)
			{
				uGetWindowText(value.edit, *value.setting);
			}
		}

		void on_change()
		{
			for (auto& [key, value] : m_check_map)
			{
				if (value.id) {
					value.check.EnableWindow(m_check_map.at(value.id).check.IsChecked());
				}
			}

			for (auto& [key, value] : m_str_map)
			{
				value.edit.EnableWindow(m_check_map.at(value.id).check.IsChecked());
			}

			m_callback->on_state_changed();
		}

		void reset() override
		{
			for (auto& [key, value] : m_check_map)
			{
				value.check.SetCheck(value.def);
			}

			for (auto& [key, value] : m_str_map)
			{
				uSetWindowText(value.edit, value.def);
				value.edit.EnableWindow(m_check_map.at(value.id).def);
			}

			on_change();
		}

		void OnChanged(UINT, int, CWindow)
		{
			on_change();
		}

	private:
		struct check_cfg
		{
			cfg_bool* setting;
			bool def;
			int id;
			CCheckBox check;
		};

		struct str_cfg
		{
			cfg_string* setting;
			const char* def;
			int id;
			CEdit edit;
		};

		preferences_page_callback::ptr m_callback;
		std::map<int, check_cfg> m_check_map;
		std::map<int, str_cfg> m_str_map;
	};

	class my_preferences_page_impl : public preferences_page_impl<my_preferences_page_instance>
	{
	public:
		GUID get_guid() override
		{
			static constexpr GUID guid = { 0x79179a37, 0x5942, 0x4fdf,{ 0xbb, 0xb7, 0x93, 0xfd, 0x35, 0xfc, 0xfe, 0x97 } };
			return guid;
		}

		GUID get_parent_guid() override
		{
			return preferences_page::guid_tagging;
		}

		bool get_help_url(pfc::string_base& out) override
		{
			out.set_string("https://marc2k3.github.io/foo_musicbrainz.html");
			return true;
		}

		const char* get_name() override
		{
			return component_title;
		}
	};

	FB2K_SERVICE_FACTORY(my_preferences_page_impl);
}
