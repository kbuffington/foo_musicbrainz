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
}

mbTrack::~mbTrack()
{
#ifdef _DEBUG
	console::print("mbTrack::~mbTrack");
#endif
	delete [] title;
	delete [] id;
}

char *mbTrack::getTitle()
{
#ifdef _DEBUG
	console::print("mbTrack::getTitle");
#endif
	return title;
}

char *mbTrack::getId()
{
#ifdef _DEBUG
	console::print("mbTrack::getId");
#endif
	return id;
}
