#include "foo_musicbrainz.h"

extern cfg_bool cfg_write_ids;

foo_mb_file_info_filter_impl::foo_mb_file_info_filter_impl(HWND _tagger_dialog, mbCollection *_mbc)
{
	tagger_dialog = _tagger_dialog;
	mbc = _mbc;
	release = mbc->getRelease();
	tracklist = mbc->getData();
}

bool foo_mb_file_info_filter_impl::apply_filter(metadb_handle_ptr p_location,t_filestats p_stats,file_info & p_info)
{
	char track_number_str[10];
	for (unsigned int i = 0; i < tracklist->get_count(); i++)
	{
		if (tracklist->get_item(i) != p_location) continue;

		p_info.meta_set("ALBUM", release->getTitle());
		if (strcmp(release->getDate(), "") != NULL)
		{
			p_info.meta_set("DATE", release->getDate());
		}
		p_info.meta_set("TITLE", release->getTrack(i)->getTitle());
		sprintf(track_number_str, "%u", i+1);
		p_info.meta_set("TRACKNUMBER", track_number_str);
		sprintf(track_number_str, "%u", release->getTracksCount());
		p_info.meta_set("TOTALTRACKS", track_number_str);
		if (strcmp(release->getDisc(), "") != NULL)
		{
			p_info.meta_set("DISCNUMBER", release->getDisc());
		}
		if (strcmp(release->getDiscSubtitle(), "") != NULL)
		{
			p_info.meta_set("DISCSUBTITLE", release->getDiscSubtitle());
		}
		if (cfg_write_ids)
		{
			p_info.meta_set("MUSICBRAINZ_ALBUMID", release->getId());
			p_info.meta_set("MUSICBRAINZ_TRACKID", release->getTrack(i)->getId());
			p_info.meta_set("MUSICBRAINZ_DISCID", mbc->getDiscId());
			if (strcmp(mbc->getDiscId(), "") != NULL)
			{
				p_info.meta_set("MUSICBRAINZ_DISCID", mbc->getDiscId());
			}
		}
		if (strcmp(release->Types[release->getType()], "") != NULL)
		{
			p_info.meta_set("MUSICBRAINZ_ALBUMTYPE", release->Types[release->getType()]);
		}
		if (strcmp(release->Statuses[release->getStatus()], "") != NULL)
		{
			p_info.meta_set("MUSICBRAINZ_ALBUMSTATUS", release->Statuses[release->getStatus()]);
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
			if (cfg_write_ids)
			{
				if (strcmp(release->getTrack(i)->getArtistId(), "") == 0)
				{
					p_info.meta_set("MUSICBRAINZ_ARTISTID", release->getArtistId());
				}
				else
				{
					p_info.meta_set("MUSICBRAINZ_ARTISTID", release->getTrack(i)->getArtistId());
				}
			}
		}
		else
		{
			p_info.meta_set("ARTIST", release->getArtist());
			p_info.meta_remove_field("ALBUM ARTIST");
			if (cfg_write_ids)
			{
				p_info.meta_set("MUSICBRAINZ_ARTISTID", release->getArtistId());
				p_info.meta_remove_field("MUSICBRAINZ_ALBUMARTISTID");
			}
		}
		return true;
	}
	return false;
}

foo_mb_file_info_filter_impl::~foo_mb_file_info_filter_impl()
{
	DestroyWindow(tagger_dialog);
}
