#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_track_number_error, pfc::exception, COMPONENT_TITLE ": unexpected track number.")

mbRelease::mbRelease(const char *_title, const char *_id, const char *_artist, const char *_artist_id)
: title(_title), id(_id), artist(_artist), artist_id(_artist_id)
{
	va = false;
}

mbTrack *mbRelease::addTrack(const char *_title, const char *_id)
{
	mbTrack *track = new mbTrack(_title, _id);
	tracks.add_item(track);
	return track;
}

void mbRelease::setDate(const char *_date)
{
	date = _date;
}

mbTrack *mbRelease::getTrack(unsigned int number)
{
	if (number >= tracks.get_count()) throw exception_foo_mb_track_number_error();
	return tracks[number];
}

const char *mbRelease::getArtist()
{
	return artist.get_ptr();
}

const char *mbRelease::getArtistId()
{
	return artist_id.get_ptr();
}

const char *mbRelease::getDate()
{
	return date.get_ptr();
}

const char *mbRelease::getId()
{
	return id.get_ptr();
}

const char *mbRelease::getTitle()
{
	return title.get_ptr();
}

unsigned int mbRelease::getTracksCount()
{
	return tracks.get_count();
}

mbRelease::~mbRelease()
{
	for (unsigned int i = 0; i < tracks.get_count(); i++)
	{
		delete tracks[i];
	}
}
