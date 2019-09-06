#include "stdafx.h"
#include "mb_helpers.h"

Release parser(json release, t_size handle_count)
{
	Release r;
	get_artist_credit(release, r.album_artist, r.albumartistid);
	str8 totaltracks = PFC_string_formatter() << handle_count;

	auto medias = release["media"];
	if (medias.is_array())
	{
		str8 totaldiscs = PFC_string_formatter() << medias.size();
		for (auto& media : medias)
		{
			auto tracks = media["tracks"];
			if (tracks.is_array() && tracks.size() == handle_count)
			{
				Disc d;
				str8 artist, id;
				d.is_various = false;
				for (auto& track : tracks)
				{
					Track t;
					get_artist_credit(track, t.artist, t.artistid);
					t.title = to_str(track["title"]);
					t.releasetrackid = to_str(track["id"]);
					t.track = to_str(track["position"]);
					t.totaltracks = totaltracks;

					auto recording = track["recording"];
					if (recording.is_object())
					{
						t.trackid = to_str(recording["id"]);
					}

					if (!d.is_various && !r.album_artist.equals(t.artist)) d.is_various = true;
					d.tracks.add_item(t);
				}

				d.disc = to_str(media["position"]);
				d.format = to_str(media["format"]);
				d.subtitle = to_str(media["title"]);
				d.totaldiscs = totaldiscs;
				
				r.discs.add_item(d);
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

	auto label_info = release["label-info"];
	if (label_info.is_array())
	{
		auto label = label_info[0]["label"];
		if (label.is_object())
		{
			r.label = to_str(label["name"]);
		}
		r.catalognumber = to_str(label_info[0]["catalog-number"]);
	}

	auto text_rep = release["text-representation"];
	if (text_rep.is_object())
	{
		r.language = to_str(text_rep["language"]);
		r.script = to_str(text_rep["script"]);
	}

	auto rg = release["release-group"];
	if (rg.is_object())
	{
		r.first_release_date = to_str(rg["first-release-date"]);
		r.primary_type = to_str(rg["primary-type"]);
		r.releasegroupid = to_str(rg["id"]);
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
		str8 tmp = s.c_str();
		ascii_replacer(tmp);
		return tmp;
	}
	return s.c_str();
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

void ascii_replacer(str8& out)
{
	pfc::string str(out);
	for (t_size i = 0; i < ascii_replacements_count; ++i) {
		auto what = ascii_replacements[i][0];
		auto with = ascii_replacements[i][1];
		str = str.replace(what, with);
	}
	out = str.get_ptr();
}

void get_artist_credit(json j, str8& name, str8& id)
{
	auto artist_credit = j["artist-credit"];
	if (artist_credit.is_array())
	{
		name =  to_str(artist_credit[0]["name"]);
		id = to_str(artist_credit[0]["artist"]["id"]);
	}
}

void tagger(metadb_handle_list_cref handles, Release release, t_size disc_idx)
{
	t_size count = handles.get_count();
	pfc::list_t<file_info_impl> info;
	info.set_size(count);

	auto& d = release.discs[disc_idx];

	for (t_size i = 0; i < count; ++i)
	{
		auto track = d.tracks[i];
		info[i] = handles[i]->get_info_ref()->info();

		if (mb_preferences::write_albumartist || d.is_various)
		{
			info[i].meta_set("ALBUM ARTIST", release.album_artist);
			if (mb_preferences::write_ids) info[i].meta_set("MUSICBRAINZ_ALBUMARTISTID", release.albumartistid);
		}

		info[i].meta_set("ALBUM", release.title);
		info[i].meta_set("ARTIST", track.artist);
		info[i].meta_set("TITLE", track.title);
		info[i].meta_set("TRACKNUMBER", track.track);
		info[i].meta_set("TOTALTRACKS", track.totaltracks);
		info[i].meta_set("DATE", release.date);
		if (release.first_release_date.get_length() && !release.date.equals(release.first_release_date)) info[i].meta_set("ORIGINAL RELEASE DATE", release.first_release_date);

		if (release.discs.get_count() > 1)
		{
			info[i].meta_set("DISCNUMBER", d.disc);
			info[i].meta_set("TOTALDISCS", d.totaldiscs);
			info[i].meta_set("DISCSUBTITLE", d.subtitle);
		}

		if (mb_preferences::albumtype)
		{
			if (get_type_index(release.primary_type) > 0) info[i].meta_set(mb_preferences::albumtype_data, release.primary_type);
		}

		if (mb_preferences::albumstatus)
		{
			if (get_status_index(release.status) > 0) info[i].meta_set(mb_preferences::albumstatus_data, release.status);
		}

		if (mb_preferences::write_label_info)
		{
			if (release.label.get_length()) info[i].meta_set("LABEL", release.label);
			if (release.catalognumber.get_length()) info[i].meta_set("CATALOGNUMBER", release.catalognumber);
			if (release.barcode.get_length()) info[i].meta_set("BARCODE", release.barcode);
		}

		if (mb_preferences::write_ids)
		{
			if (release.discid.get_length()) info[i].meta_set("MUSICBRAINZ_DISCID", release.discid);
			info[i].meta_set("MUSICBRAINZ_ALBUMID", release.albumid);
			info[i].meta_set("MUSICBRAINZ_ARTISTID", track.artistid);
			info[i].meta_set("MUSICBRAINZ_RELEASEGROUPID", release.releasegroupid);
			info[i].meta_set("MUSICBRAINZ_RELEASETRACKID", track.releasetrackid);
			info[i].meta_set("MUSICBRAINZ_TRACKID", track.trackid);
		}

		if (mb_preferences::write_country && release.country.get_length())
		{
			info[i].meta_set("RELEASECOUNTRY", release.country);
		}

		if (mb_preferences::write_format && d.format.get_length())
		{
			info[i].meta_set("MEDIA", d.format);
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
