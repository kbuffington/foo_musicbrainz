#include "foo_musicbrainz.h"

Track::Track(const char *_title, const char *_id)
: title(_title), id(_id)
{
}

const char *Track::getArtist()
{
	return artist;
}

const char *Track::getArtistId()
{
	return artist_id;
}

const char *Track::getId()
{
	return id;
}

const char *Track::getTitle()
{
	return title;
}

void Track::setArtist(const char *_artist)
{
	artist = _artist;
}

void Track::setArtistId(const char *_artist_id)
{
	artist_id = _artist_id;
}

void Track::setTitle(const char *_title)
{
	title = _title;
}
