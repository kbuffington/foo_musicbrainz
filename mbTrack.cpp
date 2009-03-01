#include "foo_musicbrainz.h"

mbTrack::mbTrack(const char *_title, const char *_id)
: title(_title), id(_id)
{
}

const char *mbTrack::getArtist()
{
	return artist.get_ptr();
}

const char *mbTrack::getArtistId()
{
	return artist_id.get_ptr();
}

const char *mbTrack::getId()
{
	return id.get_ptr();
}

const char *mbTrack::getTitle()
{
	return title.get_ptr();
}

void mbTrack::setArtist(const char *_artist)
{
	artist = _artist;
}

void mbTrack::setArtistId(const char *_artist_id)
{
	artist_id = _artist_id;
}

