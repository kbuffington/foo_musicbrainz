#pragma once

struct Track
{
	str8 artist;
	str8 release_trackid;
	str8 title;
	str8 trackid;
	str8 track;
	str8 totaltracks;
};

struct Disc
{
	pfc::list_t<Track> tracks;
	str8 disc;
	str8 format;
	str8 subtitle;
	str8 totaldiscs;
};

struct Release
{
	pfc::list_t<Disc> discs;
	str8 album_artist;
	str8 albumid;
	str8 asin;
	str8 barcode;
	str8 catalognumber;
	str8 country;
	str8 date;
	str8 discid;
	str8 first_release_date;
	str8 label;
	str8 language;
	str8 primary_type;
	str8 release_groupid;
	str8 script;
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

class mb_track_list_view : public InPlaceEdit::CTableEditHelperV2_ListView {
public:
	bool is_active();
	void abort();
	void attach(HWND wnd);
	void start(t_size item, t_size sub_item);

protected:
	HWND TableEdit_GetParentWnd() const;
	bool TableEdit_IsColumnEditable(t_size sub_item) const;
	void TableEdit_SetField(t_size item, t_size sub_item, const char* value);

private:
	HWND wnd;
};

Release parser(json release, t_size handle_count);
str8 get_artist_credit(json j);
str8 get_status_str(t_size idx);
str8 get_type_str(t_size idx);
str8 to_str(json j);
t_size get_status_index(str8 str);
t_size get_type_index(str8 str);

