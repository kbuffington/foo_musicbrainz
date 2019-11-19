#include "stdafx.h"

namespace prefs
{
	namespace check
	{
		cfg_bool server(guids::check_server, defaults::check_server);
		cfg_bool short_date(guids::check_short_date, defaults::check_short_date);
		cfg_bool ascii_punctuation(guids::check_ascii_punctuation, defaults::check_ascii_punctuation);
		cfg_bool write_ids(guids::check_write_ids, defaults::check_write_ids);
		cfg_bool albumtype(guids::check_albumtype, defaults::check_albumtype);
		cfg_bool albumstatus(guids::check_albumstatus, defaults::check_albumstatus);
		cfg_bool write_label_info(guids::check_write_label_info, defaults::check_write_label_info);
		cfg_bool write_country(guids::check_write_country, defaults::check_write_country);
		cfg_bool write_format(guids::check_write_format, defaults::check_write_format);
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
}

namespace mb
{
	class my_preferences_page_instance : public CDialogImpl<my_preferences_page_instance>, public preferences_page_instance
	{
	public:
		my_preferences_page_instance(preferences_page_callback::ptr p_callback) : m_callback(p_callback) {}

		BEGIN_MSG_MAP(my_preferences_page_instance)
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
			m_check_map[IDC_CHECK_ALBUMTYPE] = { &check::albumtype, defaults::check_albumtype };
			m_check_map[IDC_CHECK_ALBUMSTATUS] = { &check::albumstatus, defaults::check_albumstatus };
			m_check_map[IDC_CHECK_WRITE_LABEL_INFO] = { &check::write_label_info, defaults::check_write_label_info };
			m_check_map[IDC_CHECK_WRITE_COUNTRY] = { &check::write_country, defaults::check_write_country };
			m_check_map[IDC_CHECK_WRITE_FORMAT] = { &check::write_format, defaults::check_write_format };
			m_check_map[IDC_CHECK_WRITE_ASIN] = { &check::write_asin, defaults::check_write_asin };
			m_check_map[IDC_CHECK_WRITE_ISRC] = { &check::write_isrc, defaults::check_write_isrc };
			m_check_map[IDC_CHECK_WRITE_ALBUMARTIST] = { &check::write_albumartist, defaults::check_write_albumartist };

			for (auto& [key, value] : m_check_map)
			{
				value.check = GetDlgItem(key);
				value.check.SetCheck(value.setting->get_value());
			}

			m_str_map[IDC_EDIT_SERVER] = { &str::server, defaults::str_server, IDC_CHECK_SERVER };
			m_str_map[IDC_EDIT_ALBUMTYPE] = { &str::albumtype, defaults::str_albumtype, IDC_CHECK_ALBUMTYPE };
			m_str_map[IDC_EDIT_ALBUMSTATUS] = { &str::albumstatus, defaults::str_albumstatus, IDC_CHECK_ALBUMSTATUS };

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

		bool get_help_url(pfc::string_base& p_out) override
		{
			p_out = "https://marc2k3.github.io/foo_musicbrainz.html";
			return true;
		}

		const char* get_name() override
		{
			return COMPONENT_TITLE;
		}
	};

	preferences_page_factory_t<my_preferences_page_impl> g_my_preferences_page_impl;
}
