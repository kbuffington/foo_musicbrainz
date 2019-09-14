#include "stdafx.h"
#include "mb_helpers.h"

static const char* ascii_replacements[][2] = {
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

static const t_size ascii_replacements_count = PFC_TABSIZE(ascii_replacements);

Release parser(json release, t_size handle_count)
{
	Release r;
	r.is_various = false;

	get_artist_credit(release, r.album_artist, r.albumartistid);

	auto medias = release["media"];
	if (medias.is_array())
	{
		r.totaltracks = get_release_totaltracks(medias);
		r.disc_count = 0;

		for (auto& media : medias)
		{
			auto tracks = media["tracks"];
			if (tracks.is_array() && (r.totaltracks == handle_count || tracks.size() == handle_count))
			{
				str8 format = to_str(media["format"]);
				str8 subtitle = to_str(media["title"]);
				t_size discnumber = media["position"].get<t_size>();

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
					t.totaldiscs = medias.size();
					t.totaltracks = tracks.size();

					auto recording = track["recording"];
					if (recording.is_object())
					{
						t.trackid = to_str(recording["id"]);
					}

					if (!r.is_various && !r.album_artist.equals(t.artist)) r.is_various = true;
					r.tracks.add_item(t);
				}

				if (r.totaltracks != handle_count)
				{
					r.disc_count++;
				}
			}
		}
	}

	r.albumid = to_str(release["id"]);
	r.barcode = to_str(release["barcode"]);
	r.country = to_str(release["country"]);
	r.date = to_str(release["date"]);
	r.status = to_str(release["status"]);
	r.title = to_str(release["title"]);

	auto label_info = release["label-info"];
	if (label_info.is_array())
	{
		auto label = label_info[0]["label"];
		if (label.is_object())
		{
			r.label = to_str(label["name"]);
		}
		r.catalog = to_str(label_info[0]["catalog-number"]);
	}

	auto rg = release["release-group"];
	if (rg.is_object())
	{
		r.first_release_date = to_str(rg["first-release-date"]);
		r.primary_type = to_str(rg["primary-type"]);
		r.releasegroupid = to_str(rg["id"]);
	}

	if (mb_preferences::short_date && r.date.get_length() > 4)
	{
		std::string tmp = r.date.get_ptr();
		r.date = tmp.substr(0, 4).c_str();
	}

	return r;
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
	std::string s = j.type() == json::value_t::string ? j.get<std::string>() : j.dump();
	if (mb_preferences::ascii_punctuation)
	{
		pfc::string tmp(s.c_str());
		for (t_size i = 0; i < ascii_replacements_count; ++i)
		{
			auto what = ascii_replacements[i][0];
			auto with = ascii_replacements[i][1];
			tmp = tmp.replace(what, with);
		}
		return tmp.get_ptr();
	}
	return s.c_str();
}

t_size get_release_totaltracks(json j)
{
	t_size tmp = 0;
	for (auto& blah : j)
	{
		tmp += blah["tracks"].size();
	}
	return tmp;
}

t_size get_status_index(str8 str)
{
	for (t_size i = 0; i < PFC_TABSIZE(release_statuses); ++i)
	{
		if (_stricmp(str.get_ptr(), release_statuses[i]) == 0) return i;
	}
	return 0;
}

t_size get_type_index(str8 str)
{
	for (t_size i = 0; i < PFC_TABSIZE(release_group_types); ++i)
	{
		if (_stricmp(str.get_ptr(), release_group_types[i]) == 0) return i;
	}
	return 0;
}

void filter_releases(json releases , t_size count, pfc::string_list_impl& out)
{
	for (auto& release : releases)
	{
		auto id = to_str(release["id"]);
		auto track_count = release["track-count"];
		if (track_count.is_number_unsigned() && track_count.get<t_size>() == count)
		{
			out.add_item(id);
		}
		else
		{
			auto medias = release["media"];
			if (medias.is_array())
			{
				for (auto& media : medias)
				{
					auto track_count = media["track-count"];
					if (track_count.is_number_unsigned() && track_count.get<t_size>() == count)
					{
						out.add_item(id);
						break;
					}
				}
			}
		}
	}
}

void get_artist_credit(json j, str8& name, pfc::string_list_impl& ids)
{
	auto artist_credit = j["artist-credit"];
	if (artist_credit.is_array())
	{
		for (auto& artist : artist_credit)
		{
			name << to_str(artist["name"]) << to_str(artist["joinphrase"]);
			ids.add_item(to_str(artist["artist"]["id"]));
		}
	}
}

void tagger(metadb_handle_list_cref handles, Release release, t_size current_disc)
{
	t_size count = handles.get_count();
	pfc::list_t<file_info_impl> info;
	info.set_size(count);

	str8 subtitle;

	for (t_size i = 0; i < count; ++i)
	{
		info[i] = handles[i]->get_info_ref()->info();
		auto track = release.tracks[i + (current_disc * count)];

		// since editing the discsubtitle only updates track #1, apply to all tracks here
		if (track.tracknumber == 1) subtitle = track.subtitle;
		else track.subtitle = subtitle;

		if (mb_preferences::write_albumartist || release.is_various)
		{
			info[i].meta_set("ALBUM ARTIST", release.album_artist);
		}

		info[i].meta_set("ALBUM", release.title);
		info[i].meta_set("ARTIST", track.artist);
		info[i].meta_set("TITLE", track.title);
		info[i].meta_set("TRACKNUMBER", PFC_string_formatter() << track.tracknumber);
		info[i].meta_set("TOTALTRACKS", PFC_string_formatter() << track.totaltracks);
		info[i].meta_set("DATE", release.date);
		if (release.first_release_date.get_length() && !release.date.equals(release.first_release_date)) info[i].meta_set("ORIGINAL RELEASE DATE", release.first_release_date);

		if (track.totaldiscs > 1)
		{
			info[i].meta_set("DISCNUMBER", PFC_string_formatter() << track.discnumber);
			info[i].meta_set("TOTALDISCS", PFC_string_formatter() << track.totaldiscs);
			if (track.subtitle.get_length()) info[i].meta_set("DISCSUBTITLE", track.subtitle);
		}

		if (mb_preferences::albumtype && get_type_index(release.primary_type) > 0)
		{
			info[i].meta_set(mb_preferences::albumtype_data, release.primary_type);
		}

		if (mb_preferences::albumstatus && get_status_index(release.status) > 0)
		{
			info[i].meta_set(mb_preferences::albumstatus_data, release.status);
		}

		if (mb_preferences::write_label_info)
		{
			if (release.label.get_length()) info[i].meta_set("LABEL", release.label);
			if (release.catalog.get_length()) info[i].meta_set("CATALOGNUMBER", release.catalog);
			if (release.barcode.get_length()) info[i].meta_set("BARCODE", release.barcode);
		}

		if (mb_preferences::write_ids)
		{
			if (release.discid.get_length()) info[i].meta_set("MUSICBRAINZ_DISCID", release.discid);
			info[i].meta_set("MUSICBRAINZ_ALBUMID", release.albumid);
			info[i].meta_set("MUSICBRAINZ_RELEASEGROUPID", release.releasegroupid);
			info[i].meta_set("MUSICBRAINZ_RELEASETRACKID", track.releasetrackid);
			info[i].meta_set("MUSICBRAINZ_TRACKID", track.trackid);

			for (t_size j = 0; j < release.albumartistid.get_count(); ++j)
			{
				if (j == 0) info[i].meta_remove_field("MUSICBRAINZ_ALBUMARTISTID");
				info[i].meta_add("MUSICBRAINZ_ALBUMARTISTID", release.albumartistid[j]);
			}

			for (t_size j = 0; j < track.artistid.get_count(); ++j)
			{
				if (j == 0) info[i].meta_remove_field("MUSICBRAINZ_ARTISTID");
				info[i].meta_add("MUSICBRAINZ_ARTISTID", track.artistid[j]);
			}
		}

		if (mb_preferences::write_country && release.country.get_length())
		{
			info[i].meta_set("RELEASECOUNTRY", release.country);
		}

		if (mb_preferences::write_format && track.format.get_length())
		{
			info[i].meta_set("MEDIA", track.format);
		}
	}

	metadb_io_v2::get()->update_info_async_simple(
		handles,
		pfc::ptr_list_const_array_t<const file_info, file_info_impl*>(info.get_ptr(), info.get_count()),
		core_api::get_main_window(),
		metadb_io_v2::op_flag_delay_ui,
		nullptr
	);
}
