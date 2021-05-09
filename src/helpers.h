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
		pfc::string_list_impl label;
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

	static constexpr std::array<const char*, 45> album_descriptors =
	{
		"10th Anniversary",
		"20th Anniversary",
		"25th Anniversary",
		"30th Anniversary",
		"7\" Vinyl",
		"12\" Vinyl",
		"Bonus Disc",
		"Bonus Track",
		"CD1", "CD2", "CD3", "CD4",
		"CD#1", "CD#2", "CD#3", "CD#4",
		"CD #1", "CD #2", "CD #3", "CD #4",
		"Deluxe Edition",
		"Digipack",
		"Disc 1",
		"Disc 2",
		"Disc 3",
		"Disc 4",
		"Disc One",
		"Disc Two",
		"Disc Three",
		"Disc Four",
		"Expanded Edition",
		"Japan Edition",
		"Japanese Edition",
		"Limited Edition",
		"Ltd. Edition",
		"Promo",
		"Reissue",
		"Remastered",
		"Remastered Edition",
		"SACD",
		"SHM-CD"
		"Special Edition",
		"Target Exclusive",
		"UK Edition",
		"Vinyl Edition",
		"Vinyl Reissue"
	};

	Release parser(json release, size_t handle_count, pfc::stringp discid = "");
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
	uint64_t fileTimeToMilliseconds(uint64_t ft);
}
