#pragma once

#include "Release.h"

namespace foo_musicbrainz {
	class TagWriter : public file_info_filter {
	public:
		TagWriter(Release *release, pfc::list_t<metadb_handle_ptr> tracks);
		bool apply_filter(metadb_handle_ptr p_location, t_filestats p_stats, file_info & info);
		~TagWriter();

	private:
		Release *release;
		pfc::list_t<metadb_handle_ptr> tracklist;
	};
}
