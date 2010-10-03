#pragma once

class TOC
{
public:
	TOC(metadb_handle_list_cref p_data);
	char *getDiscID();
	unsigned int getNumTracks();
	~TOC();

private:
	unsigned int *tracks_lengths;
	unsigned int tracks[100];
	unsigned int num_tracks;
	unsigned int cur_track;
	unsigned int pregap;
	char *discid;
	void addTrack(int length);
	void setPregap(int length);
	void setPregap(pfc::string8 msf);
	unsigned char *rfc822_binary(void *src,unsigned long srcl,unsigned long &len);
};
