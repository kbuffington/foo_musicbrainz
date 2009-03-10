#pragma once

#define MB_RELEASE_TYPES 12
#define MB_RELEASE_STATUSES 5

class mbRelease
{
public:
	mbRelease(const char *_title, const char *_id, const char *_artist, const char *_artist_id);
	mbTrack *addTrack(const char *_title, const char *_id);
	const char *getArtist();
	const char *getArtistId();
	const char *getDate();
	const char *getId();
	unsigned int getStatus();
	const char *getTitle();
	unsigned int getType();
	mbTrack *getTrack(unsigned int number);
	unsigned int getTracksCount();
	void setArtist(const char *_artist);
	void setDate(const char *_date);
	void setStatus(unsigned int _status);
	void setTitle(const char *_title);
	void setType(unsigned int _type);
	bool va;
	~mbRelease();

	static const char *mbRelease::Statuses[];
	static const char *mbRelease::Types[];

private:
	pfc::list_t <mbTrack *> tracks;
	pfc::string8 artist;
	pfc::string8 artist_id;
	pfc::string8 date;
	pfc::string8 id;
	unsigned int status;
	pfc::string8 title;
	unsigned int type;
};
