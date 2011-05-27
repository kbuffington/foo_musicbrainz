#include <regex>
#include "foo_musicbrainz.h"
#include "ReleaseList.h"
#include "RequestURL.h"
#include "TOC.h"
#include "Track.h"
#include "QueryByMBIDDialog.h"
#include "CCustomQueryTags.h"
#include "CTaggerDialog.h"

using namespace std::tr1;
using namespace foo_musicbrainz;

extern cfg_bool cfg_write_ids;
class foo_mb_menu : public contextmenu_item_simple
{
public:
	unsigned get_num_items()
	{
		return 4;
	}

	void get_item_name(unsigned p_index,pfc::string_base & p_out)
	{
		static const char *item_name[] = {
			"Get Tags From MusicBrainz (by TOC)",
			"Get Tags From MusicBrainz (by Artist & Album)",
			"Get Tags From MusicBrainz (by MusicBrainz Album ID)",
			"Add TOC to MusicBrainz"
		};
		p_out = item_name[p_index];
	}

	void get_item_default_path(unsigned p_index,pfc::string_base & p_out)
	{
		p_out = "Tagging";
	}

	void context_command(unsigned p_index,metadb_handle_list_cref p_data,const GUID& p_caller)
	{
		unsigned int count = p_data.get_count();
		if (p_index <= 2 && count > 99)
		{
			popup_message::g_show("Please select no more than 99 tracks.", COMPONENT_TITLE, popup_message::icon_error);
			return;
		}
		switch (p_index)
		{
		case 0:
			{
				TOC toc(p_data);
				RequestURL url;
				url.AddParam("discid", toc.getDiscID());
				url.AddParam("count", count);
				url.AddParam("cdstubs", "no");

				ReleaseList *mbc = new ReleaseList();
				pfc::list_t<metadb_handle_ptr> tracks;
				tracks.add_items(p_data);
				pfc::string8 discid(toc.getDiscID());

				// new CTaggerDialog(url.GetURL(), mbc);
				break;
			}
		case 1:
			{
				pfc::string8 artist, album;
				for (t_size i = 0; i < count; i++) {
					auto item = p_data.get_item(i);
					item->metadb_lock();
					const file_info *info;
					if (!item->get_info_locked(info)) {
						item->metadb_unlock();
						return;
					}

					auto current_artist = info->meta_get("ALBUM ARTIST", 0);
					if (current_artist == nullptr) current_artist = info->meta_get("ARTIST", 0);
					auto current_album = info->meta_get("ALBUM", 0);

					if (current_artist == nullptr || current_album == nullptr) {
						item->metadb_unlock();
						break;
					}

					// Save album and artist of the first item
					if (i == 0) {
						artist = current_artist;
						album = current_album;
						if (artist.is_empty() || album.is_empty()) {
							artist.reset();
							album.reset();
							item->metadb_unlock();
							break;
						}
					// Break if artist or album of current item are different from the first one
					} else if (strcmp(artist, current_artist) != 0 || strcmp(album, current_album) != 0) {
						artist.reset();
						album.reset();
						item->metadb_unlock();
							break;
					}
					item->metadb_unlock();
				}
				pfc::list_t<metadb_handle_ptr> tracks;
				tracks.add_items(p_data);
				new CCustomQueryTags(new ReleaseList(), tracks, artist, album);
				break;
			}
		case 2:
			{
				// Stop if MBIDs are disabled
				if (!cfg_write_ids.get_value()) return;

				pfc::string8 album_id;
				for (t_size i = 0; i < count; i++) {
					auto item = p_data.get_item(i);
					item->metadb_lock();
					const file_info *info;
					if (!item->get_info_locked(info)) {
						item->metadb_unlock();
						return;
					}

					auto current_album_id = info->meta_get("MUSICBRAINZ_ALBUMID", 0);

					if (current_album_id == nullptr) {
						item->metadb_unlock();
						break;
					}

					// Save album ID of the first item
					if (i == 0) {
						album_id = current_album_id;
						regex rx("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$");
						if (!regex_search(album_id.get_ptr(), rx)) {
							album_id.reset();
							item->metadb_unlock();
							break;
						}
					// Break if album ID of current item is different from the first one
					} else if (strcmp(album_id, current_album_id) != 0) {
						album_id.reset();
						item->metadb_unlock();
						break;
					}
					item->metadb_unlock();
				}
				pfc::list_t<metadb_handle_ptr> tracks;
				tracks.add_items(p_data);
				new QueryByMBIDDialog(new ReleaseList(), tracks, album_id);
				break;
			}
		case 3:
			{
				TOC toc(p_data);

				pfc::string8 url = "http://musicbrainz.org/cdtoc/attach?toc=";
				url << toc.getTOC();
				pfc::stringcvt::string_os_from_utf8 url_converter(url);
				ShellExecute(nullptr, _T("open"), url_converter, nullptr, nullptr, SW_SHOW);
				break;
			}
		}
	}

	GUID get_item_guid(unsigned p_index)
	{
		static const GUID guid_foo_mb_menu[] = {
			{ 0x3ca8395b, 0x694e, 0x4845, { 0xb5, 0xea, 0x56, 0x30, 0x5e, 0x7c, 0x24, 0x48 } },
			{ 0x77f1f5cd, 0xf295, 0x4ef4, { 0xba, 0x7b, 0xc7, 0x70, 0xaa, 0xc6, 0xd0, 0x1e } },
			{ 0xf453e537, 0x01e9, 0x4f2d, { 0x89, 0xdc, 0x42, 0x4d, 0x0e, 0xe5, 0x72, 0xfb } },
			{ 0x4d5e632c, 0x34f3, 0x4fda, { 0x8f, 0x71, 0x35, 0xa4, 0xb2, 0x5b, 0xea, 0x94 } }
		};
		return guid_foo_mb_menu[p_index];
	}

	bool get_item_description(unsigned p_index,pfc::string_base & p_out)
	{
		static const char *item_description[] = {
			"Queries MusicBrainz server for tags for a complete CD using TOC.",
			"Queries MusicBrainz server for tags for a complete CD using Artist/Album.",
			"Queries MusicBrainz server for tags for a complete CD using MusicBrainz Album ID.",
			"Opens MusicBrainz TOC lookup page."
		};
		p_out = item_description[p_index];
		return true;
	}

	bool context_get_display(unsigned p_index,metadb_handle_list_cref p_data,pfc::string_base & p_out,unsigned & p_displayflags,const GUID & p_caller)
	{
		PFC_ASSERT(p_index>=0 && p_index<get_num_items());
		unsigned int count = p_data.get_count();
		if (count <= 99)
		{
			if (p_index == 0 || p_index == 3)
			{
				const file_info *info;
				for (t_size i = 0; i < count; i++)
				{
					p_data.get_item(i)->metadb_lock();
					if (!p_data.get_item(i)->get_info_locked(info) || info->info_get_length_samples() % 588 != 0)
					{
						p_data.get_item(i)->metadb_unlock();
						return false;
					}
					p_data.get_item(i)->metadb_unlock();
				}
			}
			if (p_index == 2 && !cfg_write_ids.get_value())
			{
				return false;
			}
			get_item_name(p_index, p_out);
			return true;
		}
		else
		{
			return false;
		}
	}
};

static contextmenu_item_factory_t<foo_mb_menu> g_foo_mb_menu_factory;
