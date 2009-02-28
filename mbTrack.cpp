#include "foo_musicbrainz.h"

mbTrack::mbTrack(const char *_title, const char *_id)
{
#ifdef _DEBUG
	console::print("mbTrack::mbTrack");
#endif
	title = new char [strlen(_title) + 1];
	strcpy(title, _title);

	id = new char [strlen(_id) + 1];
	strcpy(id, _id);

	artist = artist_id = NULL;
}

mbTrack::~mbTrack()
{
#ifdef _DEBUG
	console::print("mbTrack::~mbTrack");
#endif
	delete [] title;
	delete [] id;
	if (artist != NULL)
	{
		delete [] artist;
	}
	if (artist_id != NULL)
	{
		delete [] artist_id;
	}
}

char *mbTrack::getTitle()
{
	return title;
}

char *mbTrack::getId()
{
	return id;
}

char *mbTrack::getArtist()
{
	return (artist ? artist : "");
}

void mbTrack::setArtist(const char *_artist)
{
	if (artist != NULL)
	{
		delete [] artist;
	}
	artist = new char [strlen(_artist) + 1];
	strcpy(artist, _artist);
}

char *mbTrack::getArtistId()
{
	return (artist_id ? artist_id : "");
}

void mbTrack::setArtistId(const char *_artist_id)
{
	if (artist_id != NULL)
	{
		delete [] artist_id;
	}
	artist_id = new char [strlen(_artist_id) + 1];
	strcpy(artist_id, _artist_id);
}
