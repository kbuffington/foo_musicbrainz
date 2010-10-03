#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_track_number_error, pfc::exception, COMPONENT_TITLE ": unexpected track number.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_release_type_error, pfc::exception, COMPONENT_TITLE ": unexpected release attribute.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_release_status_error, pfc::exception, COMPONENT_TITLE ": unexpected release status.")

const char *Release::Types[] = {
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

const char *Release::Statuses[] = {
	"",
	"Official",
	"Promotion",
	"Bootleg",
	"Pseudo-Release"
};

Release::Release(const char *_title, const char *_id, const char *_artist, const char *_artist_id)
: title(_title), id(_id), artist(_artist), artist_id(_artist_id)
{
	va = false;
	type = status = 0;
}

Track *Release::addTrack(const char *_title, const char *_id)
{
	Track *track = new Track(_title, _id);
	tracks.add_item(track);
	return track;
}

const char *Release::getArtist()
{
	return artist;
}

const char *Release::getArtistId()
{
	return artist_id;
}

const char *Release::getDate()
{
	return date;
}

const char *Release::getDisc()
{
	return disc;
}

const char *Release::getDiscSubtitle()
{
	return discsubtitle;
}

const char *Release::getId()
{
	return id;
}

const char *Release::getTitle()
{
	return title;
}

unsigned int Release::getStatus()
{
	return status;
}

const char *Release::getStatusText()
{
	return Statuses[status];
}

Track *Release::getTrack(unsigned int number)
{
	if (number >= tracks.get_count()) throw exception_foo_mb_track_number_error();
	return tracks[number];
}

unsigned int Release::getTracksCount()
{
	return tracks.get_count();
}

unsigned int Release::getType()
{
	return type;
}

const char *Release::getTypeText()
{
	return Types[type];
}

void Release::setArtist(const char *_artist)
{
	artist = _artist;
}

void Release::setDate(const char *_date)
{
	date = _date;
}

void Release::setDisc(const char *_disc)
{
	disc = _disc;
}

void Release::setDiscSubtitle(const char *_discsubtitle)
{
	discsubtitle = _discsubtitle;
}

void Release::setStatus(unsigned int _status)
{
	if (_status >= MB_RELEASE_STATUSES) throw exception_foo_mb_release_status_error();
	status = _status;
}

void Release::setStatus(const char *_status)
{
	for (unsigned int i = 0; i < MB_RELEASE_STATUSES; i++)
	{
		if (_stricmp(_status, Statuses[i]) == 0)
		{
			setStatus(i);
			break;
		}
	}
}

void Release::setTitle(const char *_title)
{
	title = _title;
}

void Release::setType(unsigned int _type)
{
	if (_type >= MB_RELEASE_TYPES) throw exception_foo_mb_release_type_error();
	type = _type;
}

void Release::setType(const char *_type)
{
	for (unsigned int i = 0; i < MB_RELEASE_TYPES; i++)
	{
		if (_stricmp(_type, Types[i]) == 0)
		{
			setType(i);
			break;
		}
	}
}

Release::~Release()
{
	for (unsigned int i = 0; i < tracks.get_count(); i++)
	{
		delete tracks[i];
	}
}
