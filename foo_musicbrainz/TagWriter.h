#pragma once

#include "foo_musicbrainz.h"
#include "FileTagMap.h"

namespace foo_musicbrainz {
	class TagWriter : public file_info_filter {
	public:
		TagWriter(FileTagMap files);
		bool apply_filter(metadb_handle_ptr p_location, t_filestats p_stats, file_info & info);

	private:
		FileTagMap files;
	};
}
