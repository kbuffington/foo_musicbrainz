#pragma once

#include "Release.h"
#include "ReleaseList.h"

namespace foo_musicbrainz {
	class TagWriter : public file_info_filter {
	public:
		TagWriter(ReleaseList *mbc, pfc::list_t<metadb_handle_ptr> tracks, size_t selected_release);
		bool apply_filter(metadb_handle_ptr p_location, t_filestats p_stats, file_info & info);
		~TagWriter();

	private:
		Release *release;
		ReleaseList *mbc;
		pfc::list_t<metadb_handle_ptr> tracklist;
	};
}
