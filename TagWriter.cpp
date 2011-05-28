#include "foo_musicbrainz.h"
#include "TagWriter.h"
#include "Track.h"

using namespace foo_musicbrainz;

TagWriter::TagWriter(Release *release, pfc::list_t<metadb_handle_ptr> tracks)
	: release(release),
	tracklist(tracks)
{
}

bool TagWriter::apply_filter(metadb_handle_ptr p_location, t_filestats p_stats, file_info & p_info) {
	//char track_number_str[10];
	//for (unsigned int i = 0; i < tracklist->get_count(); i++) {
	//	if (tracklist->get_item(i) != p_location) continue;

	//	p_info.meta_set("ALBUM", release->get_title());
	//	pfc::string8 date = release->get_date();
	//	if (!date.is_empty()) {
	//		p_info.meta_set("DATE", date);
	//	}
	//	p_info.meta_set("TITLE", release->getTrack(i)->getTitle());
	//	sprintf(track_number_str, "%u", i+1);
	//	p_info.meta_set("TRACKNUMBER", track_number_str);
	//	sprintf(track_number_str, "%u", release->getTracksCount());
	//	p_info.meta_set("TOTALTRACKS", track_number_str);
	//	if (strcmp(release->getDisc(), "") != 0) {
	//		p_info.meta_set("DISCNUMBER", release->getDisc());
	//	}
	//	if (strcmp(release->getDiscSubtitle(), "") != 0) {
	//		p_info.meta_set("DISCSUBTITLE", release->getDiscSubtitle());
	//	}
	//	if (cfg_write_ids) {
	//		p_info.meta_set("MUSICBRAINZ_ALBUMID", release->getId());
	//		p_info.meta_set("MUSICBRAINZ_TRACKID", release->getTrack(i)->getId());
	//		if (strcmp(mbc->getDiscId(), "") != 0)
	//		{
	//			p_info.meta_set("MUSICBRAINZ_DISCID", mbc->getDiscId());
	//		}
	//	}
	//	if (strcmp(release->getTypeText(), "") != 0 && cfg_albumtype) {
	//		p_info.meta_set(cfg_albumtype_data, release->get_release_group()->get_type());
	//	}
	//	if (cfg_albumstatus) {
	//		auto album_status = release->get_status();
	//		if (!album_status.is_empty()) {
	//			p_info.meta_set(cfg_albumstatus_data, album_status);
	//		}
	//	}
	//	if (release->va)
	//	{
	//		p_info.meta_set("ALBUM ARTIST", release->getArtist());
	//		if (cfg_write_ids) p_info.meta_set("MUSICBRAINZ_ALBUMARTISTID", release->getArtistId());
	//		if (strcmp(release->getTrack(i)->getArtist(), "") == 0)
	//		{
	//			p_info.meta_set("ARTIST", release->getArtist());
	//		}
	//		else
	//		{
	//			p_info.meta_set("ARTIST", release->getTrack(i)->getArtist());
	//		}
	//		if (cfg_write_ids)
	//		{
	//			if (strcmp(release->getTrack(i)->getArtistId(), "") == 0)
	//			{
	//				p_info.meta_set("MUSICBRAINZ_ARTISTID", release->getArtistId());
	//			}
	//			else
	//			{
	//				p_info.meta_set("MUSICBRAINZ_ARTISTID", release->getTrack(i)->getArtistId());
	//			}
	//		}
	//	}
	//	else {
	//		p_info.meta_set("ARTIST", release->get_artist_credit()->get_name());
	//		p_info.meta_remove_field("ALBUM ARTIST");
	//		if (cfg_write_ids) {
	//			p_info.meta_set("MUSICBRAINZ_ARTISTID", release->get_artist_credit()->get_id());
	//			p_info.meta_remove_field("MUSICBRAINZ_ALBUMARTISTID");
	//		}
	//	}
	//	return true;
	//}
	return false;
}

TagWriter::~TagWriter() {
	delete release;
}
