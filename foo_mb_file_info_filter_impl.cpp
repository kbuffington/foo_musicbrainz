#include "foo_musicbrainz.h"

foo_mb_file_info_filter_impl::foo_mb_file_info_filter_impl(HWND _tagger_dialog)
{
	tagger_dialog = _tagger_dialog;
	collection = ((mbCollection *)GetProp(tagger_dialog, L"Collection"));
	release = collection->getRelease(collection->getCurrentRelease());
	tracklist = collection->getData();
}

bool foo_mb_file_info_filter_impl::apply_filter(metadb_handle_ptr p_location,t_filestats p_stats,file_info & p_info)
{
	char track_number_str[10];
	for (unsigned int i = 0; i < tracklist->get_count(); i++)
	{
		if (tracklist->get_item(i) != p_location) continue;

		p_info.meta_set("ALBUM", release->getTitle());
		p_info.meta_set("DATE", release->getDate());
		p_info.meta_set("TITLE", release->getTrack(i)->getTitle());
		sprintf(track_number_str, "%u", i+1);
		p_info.meta_set("TRACKNUMBER", track_number_str);
		sprintf(track_number_str, "%u", release->getTracksCount());
		p_info.meta_set("TOTALTRACKS", track_number_str);
		p_info.meta_set("MUSICBRAINZ_ALBUMID", release->getId());
		p_info.meta_set("MUSICBRAINZ_TRACKID", release->getTrack(i)->getId());
		p_info.meta_set("MUSICBRAINZ_DISCID", collection->getDiscId());
		if (strcmp(collection->getDiscId(), "") != NULL)
		{
			p_info.meta_set("MUSICBRAINZ_DISCID", collection->getDiscId());
		}
		if (release->va)
		{
			p_info.meta_set("ALBUM ARTIST", release->getArtist());
			p_info.meta_set("MUSICBRAINZ_ALBUMARTISTID", release->getArtistId());
			if (strcmp(release->getTrack(i)->getArtist(), "") == 0)
			{
				p_info.meta_set("ARTIST", release->getArtist());
			}
			else
			{
				p_info.meta_set("ARTIST", release->getTrack(i)->getArtist());
			}
			if (strcmp(release->getTrack(i)->getArtistId(), "") == 0)
			{
				p_info.meta_set("MUSICBRAINZ_ARTISTID", release->getArtistId());
			}
			else
			{
				p_info.meta_set("MUSICBRAINZ_ARTISTID", release->getTrack(i)->getArtistId());
			}
		}
		else
		{
			p_info.meta_set("ARTIST", release->getArtist());
			p_info.meta_set("MUSICBRAINZ_ARTISTID", release->getArtistId());
			p_info.meta_remove_field("ALBUM ARTIST");
			p_info.meta_remove_field("MUSICBRAINZ_ALBUMARTISTID");
		}
		return true;
	}
	return false;
}

foo_mb_file_info_filter_impl::~foo_mb_file_info_filter_impl()
{
	DestroyWindow(tagger_dialog);
}
