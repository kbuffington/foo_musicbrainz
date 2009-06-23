#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_release_index_error, pfc::exception, "ReleaseList: unexpected release index.")

ReleaseList::ReleaseList(metadb_handle_list_cref _p_data)
{
	current_release = 0;
	p_data.set_count(_p_data.get_count());
	for (unsigned int i = 0; i < _p_data.get_count(); i++)
	{
		p_data[i] = _p_data[i];
	}
}

Release *ReleaseList::addRelease(const char *title, const char *id, const char *artist, const char *artist_id)
{
	Release *release = new Release(title, id, artist, artist_id);
	releases.add_item(release);
	return release;
}

unsigned int ReleaseList::getCurrentRelease()
{
	return current_release;
}

pfc::list_base_t<metadb_handle_ptr> *ReleaseList::getData()
{
	return &p_data;
}

const char *ReleaseList::getDiscId()
{
	return disc_id.get_ptr();
}

Release *ReleaseList::getRelease()
{
	return releases[current_release];
}

Release *ReleaseList::getRelease(unsigned int i)
{
	if (i >= releases.get_count()) throw exception_foo_mb_release_index_error();
	return releases[i];
}

unsigned int ReleaseList::getReleasesCount()
{
	return releases.get_count();
}

void ReleaseList::setCurrentRelease(unsigned int release)
{
	if (release >= releases.get_count()) throw exception_foo_mb_release_index_error();
	current_release = release;
}

void ReleaseList::setDiscId(const char *_disc_id)
{
	disc_id = _disc_id;
}

ReleaseList::~ReleaseList()
{
	for (unsigned int i = 0; i < releases.get_count(); i++)
	{
		delete releases[i];
	}
}
