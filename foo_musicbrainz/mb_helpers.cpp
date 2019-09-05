#include "stdafx.h"
#include "mb_helpers.h"

Release parser(json release, t_size handle_count)
{
	Release r;
	str8 handle_count_str = PFC_string_formatter() << handle_count;
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
				for (auto& track : tracks)
				{
					Track t;
					t.artist = get_artist_credit(track);
					t.title = to_str(track["title"]);
					t.release_trackid = to_str(track["id"]);
					t.track = to_str(track["position"]);
					t.totaltracks = handle_count_str;

					auto recording = track["recording"];
					if (recording.is_object())
					{
						t.trackid = to_str(recording["id"]);
					}

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

	r.album_artist = get_artist_credit(release);
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
		r.release_groupid = to_str(rg["id"]);
	}

	return r;
}

str8 get_artist_credit(json j)
{
	auto artist_credit = j["artist-credit"];
	if (artist_credit.is_array())
	{
		return to_str(artist_credit[0]["name"]);
	}
	return "";
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
