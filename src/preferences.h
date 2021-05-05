#pragma once

namespace prefs
{
	namespace check
	{
		extern cfg_bool server;
		extern cfg_bool use_orig_date;
		extern cfg_bool short_date;
		extern cfg_bool ascii_punctuation;
		extern cfg_bool write_ids;
		extern cfg_bool write_albumid;
		extern cfg_bool write_discids;
		extern cfg_bool write_trackids;
		extern cfg_bool write_albumtype;
		extern cfg_bool write_albumstatus;
		extern cfg_bool write_label_info;
		extern cfg_bool write_country;
		extern cfg_bool write_media;
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
