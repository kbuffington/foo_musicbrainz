#pragma once

namespace mb
{
	struct Track
	{
		pfc::string_list_impl artistid;
		pfc::string_list_impl isrc;
		size_t discnumber;
		size_t tracknumber;
		size_t totaldiscs;
		size_t totaltracks;
		str8 artist;
		str8 media;
		str8 releasetrackid;
		str8 subtitle;
		str8 title;
		str8 trackid;
	};

	struct Release
	{
		bool is_various;
		pfc::string_list_impl albumartistid;
		size_t partial_lookup_matches;
		std::vector<Track> tracks;
		str8 album_artist;
		str8 albumid;
		str8 asin;
		str8 barcode;
		str8 catalog;
		str8 country;
		str8 date;
		str8 discid;
		str8 original_release_date;
		str8 label;
		str8 primary_type;
		str8 releasegroupid;
		str8 status;
		str8 title;
	};

	static constexpr std::array<const char*, 12> release_group_types =
	{
		"(None)",
		"Album",
		"Single",
		"EP",
		"Compilation",
		"Soundtrack",
		"Spokenword",
		"Interview",
		"Audiobook",
		"Live",
		"Remix",
		"Other"
	};

	static constexpr std::array<const char*, 5> release_statuses =
	{
		"(None)",
		"Official",
		"Promotion",
		"Bootleg",
		"Pseudo-Release"
	};

	Release parser(json release, size_t handle_count);
	bool is_uuid(pfc::stringp mbid);
	size_t get_status_index(pfc::stringp str);
	size_t get_type_index(pfc::stringp str);
	str8 get_status_str(size_t idx);
	str8 get_type_str(size_t idx);
	str8 media_thingy(const std::vector<Track>& tracks);
	str8 slasher(pfc::stringp one, pfc::stringp two);
	str8 to_str(json j);
	void filter_releases(json releases, size_t count, pfc::string_list_impl& out);
	void get_artist_credit(json j, str8& name, pfc::string_list_impl& ids);
	void tagger(metadb_handle_list_cref handles, Release release, size_t current_disc);
}
