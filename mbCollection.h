#pragma once

class mbCollection
{
public:
	mbCollection(metadb_handle_list_cref _p_data);
	mbRelease *addRelease(const char *title, const char *id, const char *artist, const char *artist_id);
	unsigned int getCurrentRelease();
	pfc::list_base_t<metadb_handle_ptr> *getData();
	const char *getDiscId();
	mbRelease *getRelease();
	mbRelease *getRelease(unsigned int i);
	unsigned int getReleasesCount();
	void setCurrentRelease(unsigned int release);
	void setDiscId(const char *_disc_id);
	~mbCollection();

private:
	pfc::list_t <mbRelease *> releases;
	unsigned int current_release;
	pfc::list_t<metadb_handle_ptr> p_data;
	pfc::string8 disc_id;
};
