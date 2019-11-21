#pragma once

namespace prefs
{
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

	str8 get_server();
}
