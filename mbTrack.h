#pragma once

class mbTrack
{
public:
	mbTrack(const char *_title, const char *_id);
	const char *getArtist();
	const char *getArtistId();
	const char *getId();
	const char *getTitle();
	void setArtist(const char *_artist);
	void setArtistId(const char *_artist_id);
	void setTitle(const char *_title);

private:
	pfc::string8 artist;
	pfc::string8 artist_id;
	pfc::string8 id;
	pfc::string8 title;
};
