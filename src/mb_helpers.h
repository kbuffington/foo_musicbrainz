#pragma once

struct Track
{
	pfc::string_list_impl artistid;
	pfc::string_list_impl isrc;
	str8 artist;
	str8 format;
	str8 releasetrackid;
	str8 subtitle;
	str8 title;
	str8 trackid;
	t_size discnumber;
	t_size tracknumber;
	t_size totaldiscs;
	t_size totaltracks;
};

struct Release
{
	bool is_various;
	pfc::string_list_impl albumartistid;
	std::vector<Track> tracks;
	str8 album_artist;
	str8 albumid;
	str8 asin;
	str8 barcode;
	str8 catalog;
	str8 country;
	str8 date;
	str8 discid;
	str8 first_release_date;
	str8 label;
	str8 primary_type;
	str8 releasegroupid;
	str8 status;
	str8 title;
	t_size partial_lookup_matches;
};

static const char* release_group_types[] = {
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

static const char* release_statuses[] = {
	"(None)",
	"Official",
	"Promotion",
	"Bootleg",
	"Pseudo-Release"
};

Release parser(json release, t_size handle_count);
str8 format_thingy(const std::vector<Track>& tracks);
str8 get_server();
str8 get_status_str(t_size idx);
str8 get_type_str(t_size idx);
str8 slasher(const str8& one, const str8& two);
str8 to_str(json j);
t_size get_release_totaltracks(json j);
t_size get_status_index(str8 str);
t_size get_type_index(str8 str);
void filter_releases(json releases, t_size count, pfc::string_list_impl& out);
void get_artist_credit(json j, str8& name, pfc::string_list_impl& ids);
void tagger(metadb_handle_list_cref handles, Release release, t_size current_disc);
