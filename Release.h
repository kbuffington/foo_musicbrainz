#pragma once

#include "Track.h"

#define MB_RELEASE_TYPES 12
#define MB_RELEASE_STATUSES 5

class Release
{
public:
	Release(const char *_title, const char *_id, const char *_artist, const char *_artist_id);
	Track *addTrack(const char *_title, const char *_id);
	const char *getArtist();
	const char *getArtistId();
	const char *getDate();
	const char *getDisc();
	const char *getDiscSubtitle();
	const char *getId();
	unsigned int getStatus();
	const char *getStatusText();
	const char *getTitle();
	unsigned int getType();
	const char *getTypeText();
	Track *getTrack(unsigned int number);
	unsigned int getTracksCount();
	void setArtist(const char *_artist);
	void setDate(const char *_date);
	void setDisc(const char *_disc);
	void setDiscSubtitle(const char *_discsubtitle);
	void setStatus(unsigned int _status);
	void setStatus(const char *_status);
	void setTitle(const char *_title);
	void setType(unsigned int _type);
	void setType(const char *_type);
	bool va;
	~Release();

	static const char *Release::Statuses[];
	static const char *Release::Types[];

private:
	pfc::list_t <Track *> tracks;
	pfc::string8 artist;
	pfc::string8 artist_id;
	pfc::string8 date;
	pfc::string8 disc;
	pfc::string8 discsubtitle;
	pfc::string8 id;
	unsigned int status;
	pfc::string8 title;
	unsigned int type;
};


#include "foo_musicbrainz.h"
#include "CoreEntity.h"
#include "ArtistCredit.h"
#include "Date.h"

namespace foo_musicbrainz {
	class Release : public CoreEntity {
		STRING_MEMBER(title)
		MEMBER_BY_REFERENCE(Date, date)
		STRING_MEMBER(barcode)
		STRING_MEMBER(status)
		STRING_MEMBER(country)
		STRING_MEMBER(asin)

	public:
		Release() {};
	};
}
