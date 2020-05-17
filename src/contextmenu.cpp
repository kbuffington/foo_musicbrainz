#include "stdafx.h"
#include "dialog_mbid.h"
#include "dialog_tags.h"
#include "query.h"
#include "request_thread.h"
#include "toc.h"

namespace mb
{
	static constexpr std::array<const GUID, 5> context_guids =
	{
		0x3ca8395b, 0x694e, 0x4845, { 0xb5, 0xea, 0x56, 0x30, 0x5e, 0x7c, 0x24, 0x48 },
		0x77f1f5cd, 0xf295, 0x4ef4, { 0xba, 0x7b, 0xc7, 0x70, 0xaa, 0xc6, 0xd0, 0x1e },
		0xf453e537, 0x01e9, 0x4f2d, { 0x89, 0xdc, 0x42, 0x4d, 0x0e, 0xe5, 0x72, 0xfb },
		0xd74ebc1b, 0x7529, 0x4c68, { 0xb8, 0x85, 0xbf, 0xb1, 0x34, 0x8c, 0xed, 0xc8 },
		0x4d5e632c, 0x34f3, 0x4fda, { 0x8f, 0x71, 0x35, 0xa4, 0xb2, 0x5b, 0xea, 0x94 }
	};

	static const std::vector<std::pair<str8, str8>> context_names =
	{
		{ "Get tags from MusicBrainz (by TOC)", "Queries MusicBrainz server for tags for a complete CD using TOC." },
		{ "Get tags from MusicBrainz (by artist & album)","Queries MusicBrainz server for tags for a complete CD using Artist/Album." },
		{ "Get tags from MusicBrainz (by MB album ID)","Queries MusicBrainz server for tags for a complete CD using MusicBrainz Album ID." },
		{ "Get tags from MusicBrainz (by MB release group ID)","Queries MusicBrainz server for tags for a complete CD using MusicBrainz Release Group ID." },
		{ "Add TOC to MusicBrainz", "Opens MusicBrainz TOC lookup page." }
	};

	class my_contextmenu_item_simple : public contextmenu_item_simple
	{
	public:
		GUID get_item_guid(uint32_t index) override
		{
			return context_guids[index];
		}

		GUID get_parent() override
		{
			return contextmenu_groups::tagging;
		}

		bool context_get_display(uint32_t index, metadb_handle_list_cref handles, pfc::string_base& out, uint32_t& displayflags, const GUID& caller) override
		{
			bool result = false;
			switch (index)
			{
			case 0:
				result = check_count(handles) && check_samplerate(handles);
				break;
			case 1:
			case 2:
			case 3:
				result = true;
				break;
			case 4:
				result = check_count(handles) && check_lossless(handles);
				break;
			}
			if (result)
			{
				get_item_name(index, out);
			}
			return result;
		}

		bool get_item_description(uint32_t index, pfc::string_base& out) override
		{
			out = context_names[index].second;
			return true;
		}

		size_t get_num_items() override
		{
			return context_names.size();
		}

		void context_command(size_t index, metadb_handle_list_cref handles, const GUID& caller) override
		{
			HWND wnd = core_api::get_main_window();
			const size_t count = handles.get_count();
			static constexpr size_t flags = threaded_process::flag_show_progress | threaded_process::flag_show_abort | threaded_process::flag_show_delayed;
			switch (index)
			{
			case 0:
				{
					if (!check_count(handles)) return popup_message::g_show("Please select no more than 99 tracks.", component_title, popup_message::icon_error);
					if (!check_samplerate(handles)) return popup_message::g_show("The sample rate of each track must match and be either 44100 Hz or 48000 Hz. Also, the number of samples must match CD frame boundaries.", component_title, popup_message::icon_error);

					toc t(handles);
					auto q = std::make_unique<query>("discid", t.get_discid());
					q->add_param("cdstubs", "no");
					q->add_param("inc", "artists+labels+recordings+release-groups+artist-credits+isrcs");
					auto cb = fb2k::service_new<request_thread>(request_thread::types::discid, std::move(q), handles);
					threaded_process::get()->run_modeless(cb, flags, wnd, "Querying data from MusicBrainz");
				}
				break;
			case 1:
				{
					str8 artist, album;
					for (size_t i = 0; i < count; i++)
					{
						const file_info_impl info = handles.get_item(i)->get_info_ref()->info();

						auto current_artist = info.meta_get("ALBUM ARTIST", 0);
						if (current_artist == nullptr) current_artist = info.meta_get("ARTIST", 0);

						auto current_album = info.meta_get("ALBUM", 0);
						if (current_album == nullptr && count == 1) current_album = info.meta_get("TITLE", 0);

						if (current_artist == nullptr || current_album == nullptr)
						{
							artist.reset();
							album.reset();
							break;
						}
						else
						{
							if (i == 0)
							{
								artist = current_artist;
								album = current_album;
							}
							else if (strcmp(artist, current_artist) != 0 || strcmp(album, current_album) != 0)
							{
								artist.reset();
								album.reset();
								break;
							}
						}
					}

					modal_dialog_scope scope;
					if (scope.can_create())
					{
						scope.initialize(wnd);
						dialog_tags dlg(artist, album);
						if (dlg.DoModal(wnd) == IDOK)
						{
							str8 search;
							search << "artist:\"" << dlg.m_artist_str << "\"";
							search << " AND release:\"" << dlg.m_album_str << "\"";

							str8 encoded_search;
							pfc::urlEncode(encoded_search, search);

							auto q = std::make_unique<query>("release");
							q->add_param("query", encoded_search);
							q->add_param("limit", "100");
							auto cb = fb2k::service_new<request_thread>(request_thread::types::search, std::move(q), handles);
							threaded_process::get()->run_modeless(cb, flags, wnd, "Querying data from MusicBrainz");
						}
					}
				}
				break;
			case 2:
				{
					str8 album_id;
					for (size_t i = 0; i < count; i++)
					{
						const file_info_impl info = handles.get_item(i)->get_info_ref()->info();

						auto current_album_id = info.meta_get("MUSICBRAINZ_ALBUMID", 0);
						if (current_album_id == nullptr) current_album_id = info.meta_get("MUSICBRAINZ ALBUM ID", 0);

						if (current_album_id == nullptr)
						{
							album_id.reset();
							break;
						}
						else
						{
							if (i == 0)
							{
								if (is_uuid(current_album_id))
								{
									album_id = current_album_id;
								}
								else
								{
									break;
								}
							}
							else if (strcmp(album_id, current_album_id) != 0)
							{
								album_id.reset();
								break;
							}
						}
					}

					modal_dialog_scope scope;
					if (scope.can_create())
					{
						scope.initialize(wnd);
						dialog_mbid dlg(album_id, "MusicBrainz Album ID");
						if (dlg.DoModal(wnd) == IDOK)
						{
							auto q = std::make_unique<query>("release", dlg.m_mbid_str);
							q->add_param("inc", "artists+labels+recordings+release-groups+artist-credits+isrcs");
							auto cb = fb2k::service_new<request_thread>(request_thread::types::albumid, std::move(q), handles);
							threaded_process::get()->run_modeless(cb, flags, wnd, "Querying data from MusicBrainz");
						}
					}
				}
				break;
			case 3:
				{
					str8 relgroup_id;
					for (size_t i = 0; i < count; i++)
					{
						const file_info_impl info = handles.get_item(i)->get_info_ref()->info();

						auto current_relgroup_id = info.meta_get("MUSICBRAINZ_RELEASEGROUPID", 0);
						if (current_relgroup_id == nullptr) current_relgroup_id = info.meta_get("MUSICBRAINZ RELEASEGROUPID", 0);

						if (current_relgroup_id == nullptr)
						{
							relgroup_id.reset();
							break;
						}
						else
						{
							if (i == 0)
							{
								if (is_uuid(current_relgroup_id))
								{
									relgroup_id = current_relgroup_id;
								}
								else
								{
									break;
								}
							}
							else if (strcmp(relgroup_id, current_relgroup_id) != 0)
							{
								relgroup_id.reset();
								break;
							}
						}
					}

					modal_dialog_scope scope;
					if (scope.can_create())
					{
						scope.initialize(wnd);
						dialog_mbid dlg(relgroup_id, "MusicBrainz Release Group ID");
						if (dlg.DoModal(wnd) == IDOK)
						{
							auto q = std::make_unique<query>("release-group", dlg.m_mbid_str);
							q->add_param("inc", "releases+media");
							auto cb = fb2k::service_new<request_thread>(request_thread::types::search, std::move(q), handles);
							threaded_process::get()->run_modeless(cb, flags, wnd, "Querying data from MusicBrainz");
						}
					}
				}
				break;
			case 4:
				{
					if (!check_count(handles)) return popup_message::g_show("Please select no more than 99 tracks.", component_title, popup_message::icon_error);
					if (!check_lossless(handles)) return popup_message::g_show("Only lossless files with a sample rate of 44100Hz may be used for TOC submissions. Also, the number of samples must match CD frame boundaries.", component_title, popup_message::icon_error);

					toc t(handles);
					ShellExecute(nullptr, _T("open"), string_wide_from_utf8_fast(t.get_toc_url()), nullptr, nullptr, SW_SHOW);
				}
				break;
			}
		}

		void get_item_name(size_t index, pfc::string_base& out) override
		{
			out = context_names[index].first;
		}

	private:
		bool check_count(metadb_handle_list_cref handles)
		{
			return handles.get_count() <= 99;
		}

		bool check_lossless(metadb_handle_list_cref handles)
		{
			const size_t count = handles.get_count();
			for (size_t i = 0; i < count; ++i)
			{
				const file_info_impl info = handles.get_item(i)->get_info_ref()->info();
				if (info.is_encoding_lossy()) return false;
				if (info.info_get_int("samplerate") != 44100) return false;
				if (info.info_get_length_samples() % 588 != 0) return false;
			}
			return true;
		}

		bool check_samplerate(metadb_handle_list_cref handles)
		{
			const int64_t srate = handles.get_item(0)->get_info_ref()->info().info_get_int("samplerate");
			if (srate != 44100 && srate != 48000) return false;

			const size_t count = handles.get_count();
			const uint32_t bits = srate == 48000 ? 640 : 588;

			for (size_t i = 0; i < count; ++i)
			{
				const file_info_impl info = handles.get_item(i)->get_info_ref()->info();
				if (info.info_get_int("samplerate") != srate) return false;
				if (info.info_get_length_samples() % bits != 0) return false;
			}
			return true;
		}
	};

	FB2K_SERVICE_FACTORY(my_contextmenu_item_simple);
}
