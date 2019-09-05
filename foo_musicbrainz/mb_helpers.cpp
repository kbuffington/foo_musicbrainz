#include "stdafx.h"
#include "mb_helpers.h"

Release parser(json release, t_size handle_count)
{
	Release r;
	str8 handle_count_str = PFC_string_formatter() << handle_count;
	auto medias = release["media"];
	if (medias.is_array())
	{
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
				d.format = to_str(media["format"]);
				r.discs.add_item(d);
			}
		}
	}

	r.album_artist = get_artist_credit(release);
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

str8 to_str(json j)
{
	if (j.is_null()) return "";
	std::string s = j.type() == json::value_t::string ? j.get<std::string>() : j.dump();
	return s.c_str();
}
