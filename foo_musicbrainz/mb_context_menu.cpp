#include "stdafx.h"
#include "dialog_mbid.h"
#include "dialog_tags.h"
#include "mb_query.h"
#include "mb_request_thread.h"
#include "mb_toc.h"

class mb_context_menu : public contextmenu_item_simple
{
public:
	t_size get_num_items()
	{
		return 4;
	}

	void get_item_name(t_size p_index, pfc::string_base & p_out)
	{
		static const char* item_name[] = {
			"Get tags from MusicBrainz (by TOC)",
			"Get tags from MusicBrainz (by artist & album)",
			"Get tags from MusicBrainz (by MusicBrainz album ID)",
			"Add TOC to MusicBrainz"
		};
		p_out = item_name[p_index];
	}

	void get_item_default_path(t_size p_index, pfc::string_base & p_out)
	{
		p_out = "Tagging";
	}

	void context_command(t_size p_index, metadb_handle_list_cref p_data, const GUID& p_caller)
	{
		HWND wnd = core_api::get_main_window();
		t_size count = p_data.get_count();
		t_size flags = threaded_process::flag_show_progress | threaded_process::flag_show_abort | threaded_process::flag_show_delayed;
		switch (p_index)
		{
			case 0:
			{
				if (!context_check_count(p_data)) return popup_message::g_show("Please select no more than 99 tracks.", COMPONENT_TITLE, popup_message::icon_error);
				if (!context_check_samplerate(p_data)) return popup_message::g_show("The sample rate of each track must match and be either 44100 Hz or 48000 Hz. Also, the number of samples must match CD frame boundaries.", COMPONENT_TITLE, popup_message::icon_error);

				mb_toc toc(p_data);
				auto query = new mb_query("discid", toc.get_discid());
				query->add_param("cdstubs", "no");
				query->add_param("inc", "artists+labels+recordings+release-groups+artist-credits");
				auto cb = fb2k::service_new<mb_request_thread>(query, p_data);
				threaded_process::get()->run_modeless(cb, flags, wnd, "Querying data from MusicBrainz");
				break;
			}
			case 1:
			{
				str8 artist, album;
				for (t_size i = 0; i < count; i++)
				{
					auto item = p_data.get_item(i);

					auto current_artist = item->get_info_ref()->info().meta_get("ALBUM ARTIST", 0);
					if (current_artist == nullptr) current_artist = item->get_info_ref()->info().meta_get("ARTIST", 0);

					auto current_album = item->get_info_ref()->info().meta_get("ALBUM", 0);
					if (current_album == nullptr && count == 1) current_album = item->get_info_ref()->info().meta_get("TITLE", 0);

					if (current_artist == nullptr || current_album == nullptr) break;

					// Save album and artist of the first item
					if (i == 0)
					{
						artist = current_artist;
						album = current_album;
						if (artist.is_empty() || album.is_empty())
						{
							artist.reset();
							album.reset();
							break;
						}
					// Break if artist or album of current item are different from the first one
					}
					else if (strcmp(artist, current_artist) != 0 || strcmp(album, current_album) != 0)
					{
						artist.reset();
						album.reset();
						break;
					}
				}

				modal_dialog_scope scope;
				if (scope.can_create())
				{
					scope.initialize(wnd);
					dialog_tags dlg(artist, album);
					if (dlg.DoModal(wnd) == IDOK)
					{
						auto query = new mb_query("release");
						str8 search;
						search << "artist:\"" << dlg.m_artist << "\"";
						search << " AND release:\"" << dlg.m_album << "\"";
						search << " AND (tracks:" << count;
						search << " OR tracksmedium:" << count << ")";
						query->add_param("query", url_encode(search));
						query->add_param("limit", "100");
						auto cb = fb2k::service_new<mb_request_thread>(query, p_data);
						threaded_process::get()->run_modeless(cb, flags, wnd, "Querying data from MusicBrainz");
					}
				}
				break;
			}
			case 2:
			{
				str8 album_id;
				for (t_size i = 0; i < count; i++)
				{
					auto item = p_data.get_item(i);

					auto current_album_id = item->get_info_ref()->info().meta_get("MUSICBRAINZ_ALBUMID", 0);

					if (current_album_id == nullptr) break;

					// Save album ID of the first item
					if (i == 0)
					{
						album_id = current_album_id;
						regex rx("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$");
						if (!regex_search(album_id.get_ptr(), rx))
						{
							album_id.reset();
							break;
						}
					// Break if album ID of current item is different from the first one
					}
					else if (strcmp(album_id, current_album_id) != 0)
					{
						album_id.reset();
						break;
					}
				}

				modal_dialog_scope scope;
				if (scope.can_create())
				{
					scope.initialize(wnd);
					dialog_mbid dlg(album_id);
					if (dlg.DoModal(wnd) == IDOK)
					{
						auto query = new mb_query("release", dlg.m_album_id);
						query->add_param("inc", "artists+labels+recordings+release-groups+artist-credits");
						auto cb = fb2k::service_new<mb_request_thread>(query, p_data);
						threaded_process::get()->run_modeless(cb, flags, wnd, "Querying data from MusicBrainz");
					}
				}
				break;
			}
			case 3:
			{
				if (!context_check_count(p_data)) return popup_message::g_show("Please select no more than 99 tracks.", COMPONENT_TITLE, popup_message::icon_error);
				if (!context_check_lossless(p_data)) return popup_message::g_show("Only lossless files with a sample rate of 44100Hz may be used for TOC submissions. Also, the number of samples must match CD frame boundaries.", COMPONENT_TITLE, popup_message::icon_error);

				mb_toc toc(p_data);

				str8 url = "https://musicbrainz.org/cdtoc/attach?toc=";
				url << toc.get_toc();
				ShellExecute(nullptr, _T("open"), string_wide_from_utf8_fast(url), nullptr, nullptr, SW_SHOW);
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

	bool get_item_description(unsigned p_index, pfc::string_base & p_out)
	{
		static const char* item_description[] = {
			"Queries MusicBrainz server for tags for a complete CD using TOC.",
			"Queries MusicBrainz server for tags for a complete CD using Artist/Album.",
			"Queries MusicBrainz server for tags for a complete CD using MusicBrainz Album ID.",
			"Opens MusicBrainz TOC lookup page."
		};
		p_out = item_description[p_index];
		return true;
	}

	bool context_check_count(metadb_handle_list_cref p_data)
	{
		t_size count = p_data.get_count();
		if (count > 99) return false;
		return true;
	}

	bool context_check_lossless(metadb_handle_list_cref p_data)
	{
		t_size count = p_data.get_count();
		for (t_size i = 0; i < count; i++)
		{
			if (p_data.get_item(i)->get_info_ref()->info().is_encoding_lossy()) return false;

			t_int64 srate = p_data.get_item(i)->get_info_ref()->info().info_get_int("samplerate");
			t_int64 samples = p_data.get_item(i)->get_info_ref()->info().info_get_length_samples();
			if (srate != 44100 || samples % 588 != 0) return false;
		}
		return true;
	}

	bool context_check_samplerate(metadb_handle_list_cref p_data)
	{
		t_size count = p_data.get_count();
		t_int64 srate = p_data.get_item(0)->get_info_ref()->info().info_get_int("samplerate");
		for (t_size i = 0; i < count; i++)
		{
			t_int64 tmp = p_data.get_item(i)->get_info_ref()->info().info_get_int("samplerate");
			t_int64 samples = p_data.get_item(i)->get_info_ref()->info().info_get_length_samples();
			if (tmp != srate) return false;
			switch (tmp)
			{
			case 44100:
				if (samples % 588 != 0) return false;
				break;
			case 48000:
				if (samples % 640 != 0) return false;
				break;
			default:
				return false;
			}
		}
		return true;
	}

	bool context_get_display(unsigned p_index, metadb_handle_list_cref p_data, pfc::string_base & p_out, unsigned & p_displayflags, const GUID & p_caller)
	{
		PFC_ASSERT(p_index>=0 && p_index<get_num_items());
		bool result = false;
		switch (p_index)
		{
		case 0:
			result = context_check_count(p_data) && context_check_samplerate(p_data);
			break;
		case 1:
		case 2:
			result = true;
			break;
		case 3:
			result = context_check_count(p_data) && context_check_lossless(p_data);
			break;
		}
		if (result)
		{
			get_item_name(p_index, p_out);
		}
		return result;
	}
};

contextmenu_item_factory_t<mb_context_menu> g_mb_context_menu;
