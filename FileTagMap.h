#pragma once

#include "foo_musicbrainz.h"
#include "Medium.h"
#include "Release.h"
#include "Track.h"

namespace foo_musicbrainz {
	typedef pfc::list_t<pfc::string8> TagValues;

	class Tag : public pfc::map_t<pfc::string8, TagValues> {
	private:
		typedef pfc::map_t<pfc::string8, TagValues> base_class;

	public:
		Tag() {}
		Tag(Release &release, Medium &medium, Track &track);
		inline void set(pfc::string8, pfc::string8);
		inline void set(pfc::string8, const char *);
		inline void set(pfc::string8, int);
	};

	class FileTagMap : public pfc::map_t<metadb_handle_ptr, Tag>  {
	private:
		typedef pfc::map_t<metadb_handle_ptr, Tag> base_class;

	public:
		FileTagMap(Release &, pfc::list_t<metadb_handle_ptr>);
	};
}
