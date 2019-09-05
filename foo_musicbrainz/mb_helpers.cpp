#include "stdafx.h"
#include "mb_helpers.h"

HWND mb_track_list_view::TableEdit_GetParentWnd() const
{
	return wnd;
}

bool mb_track_list_view::TableEdit_IsColumnEditable(t_size sub_item) const
{
	return sub_item != 0;
}

bool mb_track_list_view::is_active()
{
	return TableEdit_IsActive();
}

void mb_track_list_view::abort()
{
	return TableEdit_Abort(true);
}

void mb_track_list_view::attach(HWND wnd)
{
	this->wnd = wnd;
}

void mb_track_list_view::set_disc(Disc* d)
{
	this->d = d;
}

void mb_track_list_view::start(t_size item, t_size sub_item)
{
	TableEdit_Start(item, sub_item);
}

void mb_track_list_view::TableEdit_SetField(t_size item, t_size sub_item, const char* value)
{
	switch (sub_item) {
	case 1:
		d->tracks[item].title = value;
		break;
	case 2:
		d->tracks[item].artist = value;
		break;
	}
	CTableEditHelperV2_ListView::TableEdit_SetField(item, sub_item, value);
}

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
				d.title = to_str(media["title"]);
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
