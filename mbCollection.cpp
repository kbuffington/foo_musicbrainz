#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_release_index_error, pfc::exception, "mbReleaseCollection: unexpected release index.")

mbCollection::mbCollection(HWND _window, metadb_handle_list_cref _p_data)
: window(_window), p_data(_p_data)
{
	releases_table = new list_view_edit(GetDlgItem(window, IDC_RELEASE_LIST));
	tracks_table = new list_view_edit(GetDlgItem(window, IDC_TRACK_LIST));
	current_release = 0;
}

mbRelease *mbCollection::addRelease(const char *title, const char *id, const char *artist, const char *artist_id)
{
	mbRelease *release = new mbRelease(title, id, artist, artist_id);
	releases.add_item(release);
	return release;
}

unsigned int mbCollection::getCurrentRelease()
{
	return current_release;
}

metadb_handle_list *mbCollection::getData()
{
	return &p_data;
}

const char *mbCollection::getDiscId()
{
	return disc_id.get_ptr();
}

mbRelease *mbCollection::getRelease(unsigned int i)
{
	if (i >= releases.get_count()) throw exception_foo_mb_release_index_error();
	return releases[i];
}

unsigned int mbCollection::getReleasesCount()
{
	return releases.get_count();
}

list_view_edit *mbCollection::getReleasesTable()
{
	return releases_table;
}

list_view_edit *mbCollection::getTracksTable()
{
	return tracks_table;
}

HWND mbCollection::getWindow()
{
	return window;
}

void mbCollection::setCurrentRelease(unsigned int release)
{
	if (release >= releases.get_count()) throw exception_foo_mb_release_index_error();
	current_release = release;
}

void mbCollection::setDiscId(const char *_disc_id)
{
	disc_id = _disc_id;
}

mbCollection::~mbCollection()
{
	for (unsigned int i = 0; i < releases.get_count(); i++)
	{
		delete releases[i];
	}
	delete releases_table;
	delete tracks_table;
}
