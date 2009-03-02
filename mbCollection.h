#pragma once

class mbCollection
{
public:
	mbCollection(HWND _window, metadb_handle_list_cref _p_data);
	mbRelease *addRelease(const char *title, const char *id, const char *artist, const char *artist_id);
	unsigned int getCurrentRelease();
	metadb_handle_list *getData();
	const char *getDiscId();
	mbRelease *getRelease(unsigned int i);
	unsigned int getReleasesCount();
	release_list_view_edit *getReleasesTable();
	track_list_view_edit *getTracksTable();
	void setCurrentRelease(unsigned int release);
	void setDiscId(const char *_disc_id);
	~mbCollection();

private:
	pfc::list_t <mbRelease *> releases;
	release_list_view_edit *releases_table;
	track_list_view_edit *tracks_table;
	unsigned int current_release;
	metadb_handle_list p_data;
	pfc::string8 disc_id;
};
