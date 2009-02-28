#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_release_index_error, pfc::exception, "mbReleaseCollection: unexpected release index.")

mbRelease *mbCollection::addRelease(const char *title, const char *id, const char *artist, const char *artist_id)
{
#ifdef _DEBUG
	console::print("mbCollection::addRelease");
#endif
	mbRelease **_releases = new mbRelease* [size+1];
	for (unsigned int i = 0; i < size; i++)
	{
		_releases[i] = releases[i];
	}
	if (size > 0)
	{
		delete [] releases;
	}
	releases = _releases;
	size++;
	disc_id = NULL;
	return releases[size-1] = new mbRelease(title, id, artist, artist_id);
}

mbRelease *mbCollection::getRelease(unsigned int i)
{
#ifdef _DEBUG
	console::print("mbCollection::getRelease");
#endif
	if (i >= size) throw exception_foo_mb_release_index_error();
	return releases[i];
}

unsigned int mbCollection::getReleasesCount()
{
#ifdef _DEBUG
	console::print("mbCollection::size");
#endif
	return size;
}

HWND mbCollection::getWindow()
{
#ifdef _DEBUG
	console::print("mbCollection::getWindow");
#endif
	return window;
}

list_view_edit *mbCollection::getReleasesTable()
{
#ifdef _DEBUG
	console::print("mbCollection::getReleasesTable");
#endif
	return releases_table;
}

list_view_edit *mbCollection::getTracksTable()
{
#ifdef _DEBUG
	console::print("mbCollection::getTracksTable");
#endif
	return tracks_table;
}

void mbCollection::setCurrentRelease(unsigned int release)
{
#ifdef _DEBUG
	console::print("mbCollection::setCurrentRelease");
#endif
	if (release < size) 
	{
		current_release = release;
	}
	else
	{
		throw exception_foo_mb_release_index_error();
	}
}

unsigned int mbCollection::getCurrentRelease()
{
#ifdef _DEBUG
	console::print("mbCollection::getCurrentRelease");
#endif
	return current_release;
}

metadb_handle_list *mbCollection::getData()
{
	return &p_data;
}

void mbCollection::setDiscId(const char *_disc_id)
{
	if (disc_id != NULL)
	{
		delete [] disc_id;
	}
	disc_id = new char [strlen(_disc_id) + 1];
	strcpy(disc_id, _disc_id);
}

char *mbCollection::getDiscId()
{
	return (disc_id ? disc_id : "");
}

mbCollection::mbCollection(HWND _window, metadb_handle_list_cref _p_data) : p_data(_p_data)
{
#ifdef _DEBUG
	console::print("mbCollection::mbCollection");
#endif
	window = _window;
	releases_table = new list_view_edit(GetDlgItem(window, IDC_RELEASE_LIST));
	tracks_table = new list_view_edit(GetDlgItem(window, IDC_TRACK_LIST));
	size = 0;
	current_release = 0;
}

mbCollection::~mbCollection()
{
#ifdef _DEBUG
	console::print("mbCollection::~mbCollection");
#endif
	for (unsigned int i = 0; i < size; i++)
	{
		delete releases[i];
	}
	if (size > 0)
	{
		delete [] releases;
	}
	if (disc_id != NULL)
	{
		delete [] disc_id;
	}
	delete releases_table;
	delete tracks_table;
}
