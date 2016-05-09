#pragma once

#include "foo_musicbrainz.h"
#include "Medium.h"
#include "Release.h"
#include "Track.h"

namespace foo_musicbrainz {
	typedef pfc::list_t<pfc::string8> TagValues;

	class Tag : public pfc::map_t<pfc::string8, TagValues> {
	public:
		Tag() {}
		Tag(const Release &release, const Medium &medium, const Track &track);
		inline void set(const pfc::string8 &key, const TagValues &value);
		inline void set(const pfc::string8 &key, const pfc::string8 &value);
		inline void set(const pfc::string8 &key, const char *value);
		inline void set(const pfc::string8 &key, int value);
		inline void unset(pfc::string8 key);

	private:
		typedef pfc::map_t<pfc::string8, TagValues> base_class;
		void write_album_type(const Release &release);
		void write_album_status(const Release &release);
		void write_artist(const Release &release, const Track &track);
		void write_date(const Release &release);
		void write_disc(const Release &release, const Medium &medium);
		void write_general(const Release &release, const Medium &medium, const Track &track);
		void write_label_info(const Release &release);
		void write_musicbrainz_ids(const Release &release, const Medium &medium, const Track &track);
	};

	class FileTagMap : public pfc::map_t<metadb_handle_ptr, Tag>  {
	public:
		FileTagMap(Release &release, pfc::list_t<metadb_handle_ptr> tracks, size_t selected_medium);

	private:
		typedef pfc::map_t<metadb_handle_ptr, Tag> base_class;
	};
}
