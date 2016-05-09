#pragma once

#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	namespace Preferences {
		extern const GUID guid_short_date;
		extern cfg_bool short_date;

		extern const GUID guid_no_feat;
		extern cfg_bool no_feat;

		extern const GUID guid_write_ids;
		extern cfg_bool write_ids;

		extern const GUID guid_albumtype;
		extern cfg_bool albumtype;

		extern const GUID guid_albumstatus;
		extern cfg_bool albumstatus;

		extern const GUID guid_albumtype_data;
		extern cfg_string albumtype_data;

		extern const GUID guid_albumstatus_data;
		extern cfg_string albumstatus_data;

		extern const GUID guid_ascii_punctuation;
		extern const bool default_ascii_punctuation;
		extern cfg_bool ascii_punctuation;

		extern const GUID guid_write_label_info;
		extern const bool default_write_label_info;
		extern cfg_bool write_label_info;
	}
}
