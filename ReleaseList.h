#pragma once

#include "Release.h"

class ReleaseList
{
public:
	ReleaseList(metadb_handle_list_cref _p_data);
	Release *addRelease(const char *title, const char *id, const char *artist, const char *artist_id);
	unsigned int getCurrentRelease();
	pfc::list_base_t<metadb_handle_ptr> *getData();
	const char *getDiscId();
	Release *getRelease();
	Release *getRelease(unsigned int i);
	unsigned int getReleasesCount();
	void setCurrentRelease(unsigned int release);
	void setDiscId(const char *_disc_id);
	~ReleaseList();

private:
	pfc::list_t <Release *> releases;
	unsigned int current_release;
	pfc::list_t<metadb_handle_ptr> p_data;
	pfc::string8 disc_id;
};
