#include "stdafx.h"

namespace mb
{
	static const std::vector<std::pair<const char*, const char*>> ascii_replacements =
	{
		{ "…", "..." },
		{ "‘", "'" },
		{ "’", "'" },
		{ "‚", "'" },
		{ "“", "\"" },
		{ "”", "\"" },
		{ "„", "\"" },
		{ "′", "'" },
		{ "″", "\"" },
		{ "‹", "<" },
		{ "›", ">" },
		{ "«", "\"" },
		{ "»", "\"" },
		{ "‐", "-" },
		{ "‒", "-" },
		{ "–", "-" },
		{ "−", "-" },
		{ "—", "-" },
		{ "―", "-" }
	};

	Release parser(json release, t_size handle_count)
	{
		Release r;
		r.is_various = false;

		get_artist_credit(release, r.album_artist, r.albumartistid);

		json medias = release.value("media", json::array());
		const t_size release_totaltracks = std::accumulate(medias.begin(), medias.end(), 0, [](t_size t, json& j) { return t + j["tracks"].size(); });
		const t_size totaldiscs = medias.size();
		const bool complete = release_totaltracks == handle_count;
		r.partial_lookup_matches = 0;

		for (auto& media : medias)
		{
			json tracks = media.value("tracks", json::array());
			if (complete || tracks.size() == handle_count)
			{
				str8 format = to_str(media["format"]);
				str8 subtitle = to_str(media["title"]);
				const t_size discnumber = media["position"].get<t_size>();
				const t_size totaltracks = tracks.size();

				for (auto& track : tracks)
				{
					Track t;
					get_artist_credit(track, t.artist, t.artistid);
					t.discnumber = discnumber;
					t.format = format;
					t.subtitle = subtitle;
					t.title = to_str(track["title"]);
					t.releasetrackid = to_str(track["id"]);
					t.tracknumber = track["position"].get<t_size>();
					t.totaldiscs = totaldiscs;
					t.totaltracks = totaltracks;

					json recording = track.value("recording", json::object());
					t.trackid = to_str(recording["id"]);

					json isrcs = recording.value("isrcs", json::array());
					for (auto& isrc : isrcs)
					{
						t.isrc.add_item(to_str(isrc));
					}

					if (!r.is_various && !r.album_artist.equals(t.artist)) r.is_various = true;
					r.tracks.emplace_back(t);
				}

				if (!complete)
				{
					r.partial_lookup_matches++;
				}
			}
		}

		r.albumid = to_str(release["id"]);
		r.asin = to_str(release["asin"]);
		r.barcode = to_str(release["barcode"]);
		r.country = to_str(release["country"]);
		r.date = to_str(release["date"]);
		r.status = to_str(release["status"]);
		r.title = to_str(release["title"]);

		json label_info = release.value("label-info", json::array());
		if (label_info.size())
		{
			auto label = label_info[0]["label"];
			if (label.is_object())
			{
				r.label = to_str(label["name"]);
			}
			r.catalog = to_str(label_info[0]["catalog-number"]);
		}

		json rg = release.value("release-group", json::object());
		r.first_release_date = to_str(rg["first-release-date"]);
		r.primary_type = to_str(rg["primary-type"]);
		r.releasegroupid = to_str(rg["id"]);

		if (prefs::check::short_date.get_value() && r.date.get_length() > 4)
		{
			std::string tmp = r.date.get_ptr();
			r.date = tmp.substr(0, 4).c_str();
		}

		return r;
	}

	str8 format_thingy(const std::vector<Track>& tracks)
	{
		str8 format;
		std::set<str8> set;
		for (const auto& track : tracks)
		{
			str8 tmp = track.format;
			if (set.count(tmp)) continue;
			set.insert(tmp);
			if (format.get_length()) format << ", ";
			format << tmp;
		}
		return format;
	}

	str8 get_status_str(t_size idx)
	{
		return release_statuses[idx];
	}

	str8 get_type_str(t_size idx)
	{
		return release_group_types[idx];
	}

	str8 slasher(const str8& one, const str8& two)
	{
		if (one.is_empty() && two.is_empty())
		{
			return "-";
		}
		return PFC_string_formatter() << one << "/" << two;
	}

	str8 to_str(json j)
	{
		if (j.is_null()) return "";
		std::string s = j.is_string() ? j.get<std::string>() : j.dump();
		if (prefs::check::ascii_punctuation.get_value())
		{
			pfc::string tmp(s.c_str());
			for (const auto& [what, with] : ascii_replacements)
			{
				tmp = tmp.replace(what, with);
			}
			return tmp.get_ptr();
		}
		return s.c_str();
	}

	t_size get_status_index(str8 str)
	{
		auto it = std::find_if(release_statuses.begin(), release_statuses.end(), [str](const str8& elem)
			{
				return _stricmp(elem, str) == 0;
			});
		if (it != release_statuses.end()) return std::distance(release_statuses.begin(), it);
		return 0;
	}

	t_size get_type_index(str8 str)
	{
		auto it = std::find_if(release_group_types.begin(), release_group_types.end(), [str](const str8& elem)
			{
				return _stricmp(elem, str) == 0;
			});
		if (it != release_group_types.end()) return std::distance(release_group_types.begin(), it);
		return 0;
	}

	void filter_releases(json releases, t_size count, pfc::string_list_impl& out)
	{
		for (auto& release : releases)
		{
			auto id = to_str(release["id"]);
			t_size track_count = release.value("track-count", std::size_t(0));
			if (track_count == count)
			{
				out.add_item(id);
			}
			else
			{
				json medias = release.value("media", json::array());
				for (auto& media : medias)
				{
					t_size track_count = media.value("track-count", std::size_t(0));
					if (track_count == count)
					{
						out.add_item(id);
						break;
					}
				}
			}
		}
	}

	void get_artist_credit(json j, str8& name, pfc::string_list_impl& ids)
	{
		json artist_credit = j.value("artist-credit", json::array());
		for (auto& artist : artist_credit)
		{
			name << to_str(artist["name"]) << to_str(artist["joinphrase"]);
			ids.add_item(to_str(artist["artist"]["id"]));
		}
	}

	void tagger(metadb_handle_list_cref handles, Release release, t_size current_disc)
	{
		t_size count = handles.get_count();
		std::vector<file_info_impl> info(count);

		bool write_albumartist = prefs::check::write_albumartist.get_value() || release.is_various;

		str8 subtitle;

		for (t_size i = 0; i < count; ++i)
		{
			info[i] = handles[i]->get_info_ref()->info();
			auto track = release.tracks[i + (current_disc * count)];

			// since editing the discsubtitle only updates track #1, apply to all tracks here
			if (track.tracknumber == 1) subtitle = track.subtitle;
			else track.subtitle = subtitle;

			if (write_albumartist)
			{
				info[i].meta_set("ALBUM ARTIST", release.album_artist);
			}

			info[i].meta_set("ALBUM", release.title);
			info[i].meta_set("ARTIST", track.artist);
			info[i].meta_set("TITLE", track.title);
			info[i].meta_set("TRACKNUMBER", std::to_string(track.tracknumber).c_str());
			info[i].meta_set("TOTALTRACKS", std::to_string(track.totaltracks).c_str());
			info[i].meta_set("DATE", release.date);
			if (release.first_release_date.get_length() && !release.date.equals(release.first_release_date)) info[i].meta_set("ORIGINAL RELEASE DATE", release.first_release_date);

			if (track.totaldiscs > 1)
			{
				info[i].meta_set("DISCNUMBER", std::to_string(track.discnumber).c_str());
				info[i].meta_set("TOTALDISCS", std::to_string(track.totaldiscs).c_str());
				if (track.subtitle.get_length()) info[i].meta_set("DISCSUBTITLE", track.subtitle);
			}

			if (prefs::check::albumtype.get_value() && get_type_index(release.primary_type) > 0)
			{
				info[i].meta_set(prefs::str::albumtype.get_ptr(), release.primary_type);
			}

			if (prefs::check::albumstatus.get_value() && get_status_index(release.status) > 0)
			{
				info[i].meta_set(prefs::str::albumstatus.get_ptr(), release.status);
			}

			if (prefs::check::write_label_info.get_value())
			{
				if (release.label.get_length()) info[i].meta_set("LABEL", release.label);
				if (release.catalog.get_length()) info[i].meta_set("CATALOGNUMBER", release.catalog);
				if (release.barcode.get_length()) info[i].meta_set("BARCODE", release.barcode);
			}

			if (prefs::check::write_ids.get_value())
			{
				if (release.discid.get_length()) info[i].meta_set("MUSICBRAINZ_DISCID", release.discid);
				info[i].meta_set("MUSICBRAINZ_ALBUMID", release.albumid);
				info[i].meta_set("MUSICBRAINZ_RELEASEGROUPID", release.releasegroupid);
				info[i].meta_set("MUSICBRAINZ_RELEASETRACKID", track.releasetrackid);
				info[i].meta_set("MUSICBRAINZ_TRACKID", track.trackid);

				for (t_size j = 0; j < track.artistid.get_count(); ++j)
				{
					if (j == 0) info[i].meta_remove_field("MUSICBRAINZ_ARTISTID");
					info[i].meta_add("MUSICBRAINZ_ARTISTID", track.artistid[j]);
				}

				if (write_albumartist)
				{
					for (t_size j = 0; j < release.albumartistid.get_count(); ++j)
					{
						if (j == 0) info[i].meta_remove_field("MUSICBRAINZ_ALBUMARTISTID");
						info[i].meta_add("MUSICBRAINZ_ALBUMARTISTID", release.albumartistid[j]);
					}
				}
			}

			if (prefs::check::write_country.get_value() && release.country.get_length())
			{
				info[i].meta_set("RELEASECOUNTRY", release.country);
			}

			if (prefs::check::write_format.get_value() && track.format.get_length())
			{
				info[i].meta_set("MEDIA", track.format);
			}

			if (prefs::check::write_asin.get_value() && release.asin.get_length())
			{
				info[i].meta_set("ASIN", release.asin);
			}

			if (prefs::check::write_isrc.get_value())
			{
				for (t_size j = 0; j < track.isrc.get_count(); ++j)
				{
					if (j == 0) info[i].meta_remove_field("ISRC");
					info[i].meta_add("ISRC", track.isrc[j]);
				}
			}
		}

		metadb_io_v2::get()->update_info_async_simple(
			handles,
			pfc::ptr_list_const_array_t<const file_info, file_info_impl*>(info.data(), info.size()),
			core_api::get_main_window(),
			metadb_io_v2::op_flag_delay_ui,
			nullptr
		);
	}
}
