#pragma once

struct Track
{
	pfc::string_list_impl artistid;
	str8 artist;
	str8 releasetrackid;
	str8 title;
	str8 trackid;
	str8 track;
	str8 totaltracks;
};

struct Disc
{
	bool is_various;
	pfc::list_t<Track> tracks;
	str8 disc;
	str8 format;
	str8 subtitle;
	str8 totaldiscs;
};

struct Release
{
	pfc::string_list_impl albumartistid;
	pfc::list_t<Disc> discs;
	str8 album_artist;
	str8 albumid;
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
str8 get_status_str(t_size idx);
str8 get_type_str(t_size idx);
str8 slasher(const str8& one, const str8& two);
str8 to_str(json j);
t_size get_status_index(str8 str);
t_size get_type_index(str8 str);
void get_artist_credit(json j, str8& name, pfc::string_list_impl& ids, t_size type);
void tagger(metadb_handle_list_cref handles, Release release, t_size disc_idx);
