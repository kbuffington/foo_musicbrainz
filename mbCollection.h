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
	list_view_edit *getReleasesTable();
	list_view_edit *getTracksTable();
	HWND mbCollection::getWindow();
	void setCurrentRelease(unsigned int release);
	void setDiscId(const char *_disc_id);
	~mbCollection();

private:
	pfc::list_t <mbRelease *> releases;
	list_view_edit *releases_table;
	list_view_edit *tracks_table;
	HWND window;
	unsigned int current_release;
	metadb_handle_list p_data;
	pfc::string8 disc_id;
};
