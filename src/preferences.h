#pragma once

namespace prefs
{
	namespace guids
	{
		static constexpr GUID check_server = { 0x2ac00b3b, 0x1b04, 0x4fb2,{ 0xa9, 0x98, 0x5c, 0x16, 0x4, 0x9c, 0xce, 0x9d } };
		static constexpr GUID check_short_date = { 0x18734618, 0x7920, 0x4d24,{ 0x84, 0xa1, 0xb9, 0xd6, 0x6e, 0xd8, 0x25, 0xbc } };
		static constexpr GUID check_ascii_punctuation = { 0xd08b1b7c, 0x38fd, 0x4689,{ 0x9e, 0x91, 0x8c, 0xdc, 0xbe, 0xc4, 0x26, 0x98 } };
		static constexpr GUID check_write_ids = { 0x8c8b61e0, 0x8eea, 0x4c34,{ 0x9a, 0x51, 0x4d, 0xa9, 0x9c, 0xec, 0xcb, 0xbc } };
		static constexpr GUID check_albumtype = { 0xd8fdb1d8, 0xde2, 0x4f1f,{ 0x85, 0x5a, 0xc0, 0x5, 0x98, 0x60, 0x9b, 0xe9 } };
		static constexpr GUID check_albumstatus = { 0x57a8dddd, 0xdf24, 0x4fd3,{ 0xac, 0x95, 0x6f, 0x8, 0x3, 0x26, 0x41, 0x7c } };
		static constexpr GUID check_write_label_info = { 0x9b3c94e3, 0x278, 0x4eb0,{ 0xa2, 0xed, 0x5, 0xad, 0xf8, 0xce, 0xa3, 0x9d } };
		static constexpr GUID check_write_country = { 0xa983fbd6, 0x7471, 0x41d7,{ 0xa7, 0x62, 0x27, 0xf9, 0x94, 0xe9, 0x23, 0x1f } };
		static constexpr GUID check_write_format = { 0x8e0f77e2, 0x50e8, 0x4e66,{ 0x88, 0xe3, 0xab, 0xed, 0x3b, 0x76, 0x84, 0x5a } };
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
		static constexpr bool check_albumtype = true;
		static constexpr bool check_albumstatus = true;
		static constexpr bool check_write_label_info = true;
		static constexpr bool check_write_country = true;
		static constexpr bool check_write_format = true;
		static constexpr bool check_write_asin = true;
		static constexpr bool check_write_isrc = true;
		static constexpr bool check_write_albumartist = false;

		static constexpr const char* str_server = "https://musicbrainz.org";
		static constexpr const char* str_albumtype = "MUSICBRAINZ_ALBUMTYPE";
		static constexpr const char* str_albumstatus = "MUSICBRAINZ_ALBUMSTATUS";
	}

	namespace check
	{
		extern cfg_bool server;
		extern cfg_bool short_date;
		extern cfg_bool ascii_punctuation;
		extern cfg_bool write_ids;
		extern cfg_bool albumtype;
		extern cfg_bool albumstatus;
		extern cfg_bool write_label_info;
		extern cfg_bool write_country;
		extern cfg_bool write_format;
		extern cfg_bool write_asin;
		extern cfg_bool write_isrc;
		extern cfg_bool write_albumartist;
	}

	namespace str
	{
		extern cfg_string server;
		extern cfg_string albumtype;
		extern cfg_string albumstatus;
	}
}
