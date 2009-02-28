#pragma once

class mbTrack {
public:
	mbTrack(const char *_title, const char *_id);
	~mbTrack();
	char *getTitle();
	char *getId();
	char *getArtist();
	void setArtist(const char *_artist);
	char *getArtistId();
	void setArtistId(const char *_artist_id);

private:
	char *id;
	char *title;
	char *artist;
	char *artist_id;
};
