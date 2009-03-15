#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_track_number_error, pfc::exception, COMPONENT_TITLE ": unexpected track number.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_release_type_error, pfc::exception, COMPONENT_TITLE ": unexpected release attribute.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_release_status_error, pfc::exception, COMPONENT_TITLE ": unexpected release status.")

const char *mbRelease::Types[] = {
	"",
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

const char *mbRelease::Statuses[] = {
	"",
	"Official",
	"Promotion",
	"Bootleg",
	"Pseudo-Release"
};

mbRelease::mbRelease(const char *_title, const char *_id, const char *_artist, const char *_artist_id)
: title(_title), id(_id), artist(_artist), artist_id(_artist_id)
{
	va = false;
	type = status = 0;
}

mbTrack *mbRelease::addTrack(const char *_title, const char *_id)
{
	mbTrack *track = new mbTrack(_title, _id);
	tracks.add_item(track);
	return track;
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

const char *mbRelease::getDisc()
{
	return disc.get_ptr();
}

const char *mbRelease::getDiscSubtitle()
{
	return discsubtitle.get_ptr();
}

const char *mbRelease::getId()
{
	return id.get_ptr();
}

const char *mbRelease::getTitle()
{
	return title.get_ptr();
}

unsigned int mbRelease::getStatus()
{
	return status;
}

mbTrack *mbRelease::getTrack(unsigned int number)
{
	if (number >= tracks.get_count()) throw exception_foo_mb_track_number_error();
	return tracks[number];
}

unsigned int mbRelease::getTracksCount()
{
	return tracks.get_count();
}

unsigned int mbRelease::getType()
{
	return type;
}

void mbRelease::setArtist(const char *_artist)
{
	artist = _artist;
}

void mbRelease::setDate(const char *_date)
{
	date = _date;
}

void mbRelease::setDisc(const char *_disc)
{
	disc = _disc;
}

void mbRelease::setDiscSubtitle(const char *_discsubtitle)
{
	discsubtitle = _discsubtitle;
}

void mbRelease::setStatus(unsigned int _status)
{
	if (_status >= MB_RELEASE_STATUSES) throw exception_foo_mb_release_status_error();
	status = _status;
}

void mbRelease::setTitle(const char *_title)
{
	title = _title;
}

void mbRelease::setType(unsigned int _type)
{
	if (_type >= MB_RELEASE_TYPES) throw exception_foo_mb_release_type_error();
	type = _type;
}

mbRelease::~mbRelease()
{
	for (unsigned int i = 0; i < tracks.get_count(); i++)
	{
		delete tracks[i];
	}
}
