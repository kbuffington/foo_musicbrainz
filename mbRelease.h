#pragma once

class mbRelease
{
public:
	mbRelease(const char *_title, const char *_id, const char *_artist, const char *_artist_id);
	mbTrack *addTrack(const char *_title, const char *_id);
	const char *getArtist();
	const char *getArtistId();
	const char *getDate();
	const char *getId();
	const char *getTitle();
	mbTrack *getTrack(unsigned int number);
	unsigned int getTracksCount();
	void setDate(const char *_date);
	bool va;
	~mbRelease();

private:
	pfc::list_t <mbTrack *> tracks;
	pfc::string8 artist;
	pfc::string8 artist_id;
	pfc::string8 date;
	pfc::string8 id;
	pfc::string8 title;
};
