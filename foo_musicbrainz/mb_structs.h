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
	str8 format;
};

struct Release
{
	pfc::list_t<Disc> discs;
	str8 album_artist;
	str8 albumid;
	str8 asin;
	str8 barcode;
	str8 country;
	str8 discid;
	str8 status;
};
