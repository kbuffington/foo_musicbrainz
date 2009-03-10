#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_release_index_error, pfc::exception, "mbReleaseCollection: unexpected release index.")

mbCollection::mbCollection(HWND window, metadb_handle_list_cref _p_data)
{
	current_release = 0;
	p_data.set_count(_p_data.get_count());
	for (unsigned int i = 0; i < _p_data.get_count(); i++)
	{
		p_data[i] = _p_data[i];
	}
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

pfc::list_base_t<metadb_handle_ptr> *mbCollection::getData()
{
	return &p_data;
}

const char *mbCollection::getDiscId()
{
	return disc_id.get_ptr();
}

mbRelease *mbCollection::getRelease()
{
	return releases[current_release];
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
}
