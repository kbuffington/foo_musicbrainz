#include "foo_musicbrainz.h"

unsigned foo_mb_menu::get_num_items()
{
	return 2;
}

void foo_mb_menu::get_item_name(unsigned p_index,pfc::string_base & p_out)
{
	static const char *item_name[] = { "Get Tags From MusicBrainz (by TOC)", "Get Tags From MusicBrainz (by data)", "Get Tags From MusicBrainz (custom query)" };
	p_out = item_name[p_index];
}

void foo_mb_menu::get_item_default_path(unsigned p_index,pfc::string_base & p_out)
{
	p_out = "Tagging";
}

void foo_mb_menu::context_command(unsigned p_index,metadb_handle_list_cref p_data,const GUID& p_caller)
{
	unsigned int count = p_data.get_count();
	if (count > 99)
	{
		popup_message::g_show("Please select no more than 99 tracks.", COMPONENT_TITLE, popup_message::icon_error);
		return;
	}
	switch (p_index)
	{
	case 0:
		{
			unsigned int *tracks_lengths = new unsigned int [count];
			const file_info *info;
			__int64 samples;
			char tracks_count[10];

			for (t_size i = 0; i < count; i++)
			{
				p_data.get_item(i)->metadb_lock();
				if (!p_data.get_item(i)->get_info_locked(info))
				{
					p_data.get_item(i)->metadb_unlock();
					delete [] tracks_lengths;
					return;
				}
				samples = info->info_get_length_samples();
				p_data.get_item(i)->metadb_unlock();
				if (samples % 588 != 0)
				{
					popup_message::g_show("Track length in samples must be divisible by 588.", COMPONENT_TITLE, popup_message::icon_error);
					delete [] tracks_lengths;
					return;
				} 
				tracks_lengths[i] = (unsigned int)samples / 588;
			}
			char *discid = get_discid(count, tracks_lengths, 150);
			delete [] tracks_lengths;
			
			pfc::string8 url = "ws/1/release/?type=xml&discid=";
			url += discid;
			url += "&count=";
			sprintf(tracks_count, "%d", count);
			url += URLEncode(tracks_count);
			delete [] discid;

			HWND tagger_dialog = CreateDialog(core_api::get_my_instance(), MAKEINTRESOURCE(IDD_TAGGER_DIALOG), core_api::get_main_window(), TaggerDialogProc);
			SetProp(tagger_dialog, L"Collection", new mbCollection(tagger_dialog, p_data));
			threaded_process::g_run_modeless(new service_impl_t<foo_mb_request_thread>(url, tagger_dialog), threaded_process::flag_show_progress | threaded_process::flag_show_abort, core_api::get_main_window(), "Quering information from MusicBrainz");
			break;
		}
	case 1:
		{
			const file_info *info;
			pfc::string8 artist, album, url;
			char tracks_count[10];
			for (t_size i = 0; i < count; i++)
			{
				p_data.get_item(i)->metadb_lock();
				if (!p_data.get_item(i)->get_info_locked(info))
				{
					p_data.get_item(i)->metadb_unlock();
					return;
				}
				artist = info->meta_get("ARTIST", 0);
				album = info->meta_get("ALBUM", 0);
				if (artist.is_empty() || album.is_empty()) continue;
				else break;
				p_data.get_item(i)->metadb_unlock();
			}
			if (artist.is_empty() || album.is_empty())
			{
				popup_message::g_show("Selected tracks do not have any Artist and Album tags.", COMPONENT_TITLE, popup_message::icon_error);
				return;
			}
			url = "ws/1/release/?type=xml&artist=";
			url += URLEncode(artist);
			url += "&title=";
			url += URLEncode(album);
			url += "&count=";
			sprintf(tracks_count, "%d", count);
			url += URLEncode(tracks_count);

			HWND tagger_dialog = CreateDialog(core_api::get_my_instance(), MAKEINTRESOURCE(IDD_TAGGER_DIALOG), core_api::get_main_window(), TaggerDialogProc);
			SetProp(tagger_dialog, L"Collection", new mbCollection(tagger_dialog, p_data));
			threaded_process::g_run_modeless(new service_impl_t<foo_mb_request_thread>(url, tagger_dialog), threaded_process::flag_show_progress | threaded_process::flag_show_abort, core_api::get_main_window(), "Quering information from MusicBrainz");
			break;
		}
	case 2:
		popup_message::g_show("Sorry, not yet implemented.", COMPONENT_TITLE, popup_message::icon_error);
		break;
	}

}

GUID foo_mb_menu::get_item_guid(unsigned p_index)
{
	static const GUID guid_foo_mb_menu[] = { { 0x3ca8395b, 0x694e, 0x4845, { 0xb5, 0xea, 0x56, 0x30, 0x5e, 0x7c, 0x24, 0x48 } }, { 0x77f1f5cd, 0xf295, 0x4ef4, { 0xba, 0x7b, 0xc7, 0x70, 0xaa, 0xc6, 0xd0, 0x1e } }, { 0x9c4dac6a, 0xe571, 0x4cd4, { 0x82, 0xd8, 0x51, 0x31, 0x44, 0x94, 0xde, 0xc5 } } };
	return guid_foo_mb_menu[p_index];
}

bool foo_mb_menu::get_item_description(unsigned p_index,pfc::string_base & p_out)
{
	static const char *item_description[] = { "Queries MusicBrainz server for tags for a complete CD using TOC.", "Queries MusicBrainz server for tags for a complete CD using tracks data.", "Queries MusicBrainz server for tags for a complete CD using custom search query." };
	p_out = item_description[p_index];
	return true;
}

bool foo_mb_menu::context_get_display(unsigned p_index,metadb_handle_list_cref p_data,pfc::string_base & p_out,unsigned & p_displayflags,const GUID & p_caller)
{
	PFC_ASSERT(p_index>=0 && p_index<get_num_items());
	unsigned int count = p_data.get_count();
	if (count <= 99)
	{
		if (p_index != 0)
		{
			get_item_name(p_index,p_out);
			return true;
		}
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
		get_item_name(p_index,p_out);
		return true;
	} else {
		return false;
	}
}

static contextmenu_item_factory_t<foo_mb_menu> g_foo_mb_menu_factory;
