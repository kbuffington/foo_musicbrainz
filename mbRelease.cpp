#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_track_number_error, pfc::exception, COMPONENT_TITLE ": unexpected track number.")

mbTrack *mbRelease::addTrack(const char *_title, const char *_id)
{
#ifdef _DEBUG
	console::print("mbRelease::addTrack");
#endif
	mbTrack **_tracks = new mbTrack* [size+1];
	for (unsigned int i = 0; i < size; i++)
	{
		_tracks[i] = tracks[i];
	}
	if (size > 0)
	{
		delete [] tracks;
	}
	tracks = _tracks;
	size++;
	printf("%d\n", size);
	return tracks[size-1] = new mbTrack(_title, _id);
}

void mbRelease::setDate(const char *_date)
{
#ifdef _DEBUG
	console::print("mbRelease::setDate");
#endif
	if (date != NULL)
	{
		delete [] date;
	}
	date = new char [strlen(_date) + 1];
	strcpy(date, _date);
}

mbTrack *mbRelease::getTrack(unsigned int number)
{
#ifdef _DEBUG
	console::print("mbRelease::getTrack");
#endif
	if (number >= size) throw exception_foo_mb_track_number_error();
	return tracks[number];
}

char *mbRelease::getTitle()
{
#ifdef _DEBUG
	console::print("mbRelease::getTitle");
#endif
	return title;
}

char *mbRelease::getId()
{
#ifdef _DEBUG
	console::print("mbRelease::getId");
#endif
	return id;
}

char *mbRelease::getArtist()
{
#ifdef _DEBUG
	console::print("mbRelease::getArtist");
#endif
	return artist;
}

char *mbRelease::getArtistId()
{
#ifdef _DEBUG
	console::print("mbRelease::getArtistId");
#endif
	return artist_id;
}

char *mbRelease::getDate()
{
#ifdef _DEBUG
	console::print("mbRelease::getDate");
#endif
	return (date ? date : "");
}

unsigned int mbRelease::getTracksCount()
{
#ifdef _DEBUG
	console::print("mbRelease::getTracksCount");
#endif
	return size;
}

mbRelease::mbRelease(const char *_title, const char *_id, const char *_artist, const char *_artist_id)
{
#ifdef _DEBUG
	console::print("mbRelease::mbRelease");
#endif
	size = 0;

	title = new char [strlen(_title) + 1];
	strcpy(title, _title);
	
	id = new char [strlen(_id) + 1];
	strcpy(id, _id);
	
	artist = new char [strlen(_artist) + 1];
	strcpy(artist, _artist);
	
	artist_id = new char [strlen(_artist_id) + 1];
	strcpy(artist_id, _artist_id);

	date = NULL;
}

mbRelease::~mbRelease()
{
#ifdef _DEBUG
	console::print("mbRelease::~mbRelease");
#endif
	for (unsigned int i = 0; i < size; i++)
	{
		delete tracks[i];
	}
	if (size > 0)
	{
		delete [] tracks;
	}
	delete [] artist;
	delete [] title;
	delete [] artist_id;
	delete [] id;
	if (date != NULL)
	{
		delete [] date;
	}
}
