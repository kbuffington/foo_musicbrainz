#pragma once

class mbCollection
{
public:
	mbCollection(HWND _window, metadb_handle_list_cref _p_data);
	~mbCollection();
	unsigned int getReleasesCount();
	mbRelease *addRelease(const char *title, const char *id, const char *artist, const char *artist_id);
	mbRelease *getRelease(unsigned int i);
	HWND mbCollection::getWindow();
	list_view_edit *getReleasesTable();
	list_view_edit *getTracksTable();
	void setCurrentRelease(unsigned int release);
	unsigned int getCurrentRelease();
	metadb_handle_list *getData();

private:
	unsigned int size;
	mbRelease **releases;
	list_view_edit *releases_table;
	list_view_edit *tracks_table;
	HWND window;
	unsigned int current_release;
	metadb_handle_list p_data;
};
