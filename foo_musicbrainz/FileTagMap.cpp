#include "foo_musicbrainz.h"
#include "FileTagMap.h"
#include "Medium.h"
#include "Release.h"
#include "Track.h"

using namespace foo_musicbrainz;
using pfc::string8;

Tag::Tag(const Release &release, const Medium &medium, const Track &track) {
	write_general(release, medium, track);
	write_date(release);
	write_disc(release, medium);
	write_album_type(release);
	write_album_status(release);
	write_artist(release, track);
	write_musicbrainz_ids(release, medium, track);
	write_label_info(release);
}

void Tag::write_general(const Release &release, const Medium &medium, const Track &track) {
	set("ALBUM", release.get_title());
	set("TITLE", track.get_title());
	set("TRACKNUMBER", track.get_position());
	set("TOTALTRACKS", medium.track_count());
}

void Tag::write_date(const Release &release) {
	string8 date = release.get_date();
	if (!date.is_empty()) {
		set("DATE", date);
	}

	string8 first_release_date = release.get_release_group()->get_first_release_date();
	if (!first_release_date.is_empty() && first_release_date != date) {
		set("ORIGINAL RELEASE DATE", first_release_date);
	} else {
		unset("ORIGINAL RELEASE DATE");
	}
}

void Tag::write_disc(const Release &release, const Medium &medium) {
	if (release.is_multidisc()) {
		set("DISCNUMBER", medium.get_position());
		set("TOTALDISCS", release.get_medium_total_count());
		set("DISCSUBTITLE", medium.get_title());
	} else {
		unset("DISCNUMBER");
		unset("TOTALDISCS");
		unset("DISCSUBTITLE");
	}
}

void Tag::write_album_type(const Release &release) {
	if (Preferences::albumtype) {
		auto type = release.get_release_group()->get_type();
		if (type != ReleaseGroup::types[0]) {
			set(Preferences::albumtype_data, type);
		} else {
			unset(Preferences::albumtype_data);
		}
	}
}

void Tag::write_album_status(const Release &release) {
	if (Preferences::albumstatus) {
		auto status = release.get_status();
		if (status != Release::statuses[0]) {
			set(Preferences::albumstatus_data, status);
		} else {
			unset(Preferences::albumstatus_data);
		}
	}
}

void Tag::write_artist(const Release &release, const Track &track) {
	set("ARTIST", track.get_artist_credit()->get_name());
	if (release.is_various()) {
		set("ALBUM ARTIST", release.get_artist_credit()->get_name());
	} else {
		unset("ALBUM ARTIST");
	}
}

void Tag::write_musicbrainz_ids(const Release &release, const Medium &medium, const Track &track) {
	if (Preferences::write_ids) {
		set("MUSICBRAINZ_ALBUMID", release.get_id());
		set("MUSICBRAINZ_RELEASEGROUPID", release.get_release_group()->get_id());
		set("MUSICBRAINZ_TRACKID", track.get_id());
		set("MUSICBRAINZ_ARTISTID", track.get_artist_credit()->get_ids());
		if (release.is_various()) {
			set("MUSICBRAINZ_ALBUMARTISTID", release.get_artist_credit()->get_ids());
		} else {
			unset("MUSICBRAINZ_ALBUMARTISTID");
		}
		auto discid = medium.get_discid();
		if (!discid.is_empty()) {
			set("MUSICBRAINZ_DISCID", discid);
		}
	}
}

void Tag::write_label_info(const Release &release) {
	if (Preferences::write_label_info) {
		set("BARCODE", release.get_barcode());

		TagValues labels, catalog_numbers;
		for (auto i = 0; i < release.label_info_count(); i++) {
			if (auto label = release.get_label_info(i)->get_name()) {
				labels.add_item(label);
			}
			labels.remove_duplicates_t(strcmp);
			if (auto catalog_number = release.get_label_info(i)->get_catalog_number()) {
				catalog_numbers.add_item(catalog_number);
			}
			catalog_numbers.remove_duplicates_t(strcmp);
		}
		set("LABEL", labels);
		set("CATALOGNUMBER", catalog_numbers);
	}
}

void Tag::set(const string8 & key, const TagValues & value) {
	base_class::set(key, value);
}

void Tag::set(const string8 & key, const string8 & value) {
	TagValues list;
	list.add_item(value);
	set(key, list);
}

void Tag::set(const string8 & key, const char *value) {
	set(key, string8(value));
}

void Tag::set(const string8 & key, int value) {
	string8 tmp;
	tmp << value;
	set(key, tmp);
}

void Tag::unset(string8 key) {
	set(key, "");
}

FileTagMap::FileTagMap(Release &release, pfc::list_t<metadb_handle_ptr> tracks, size_t selected_medium) {
	auto current_medium = 0;
	auto current_track = 0;
	if (tracks.get_count() < release.track_count()) {
		current_medium = selected_medium;
	}
	for (unsigned int i = 0; i < tracks.get_count(); i++) {
		auto &medium = *release.get_medium(current_medium);
		auto &track = *medium.get_track(current_track);

		set(tracks[i], Tag(release, medium, track));

		if (++current_track < medium.track_count()) continue;
		if (++current_medium < release.medium_count()) {
			current_track = 0;
			continue;
		}
		return;
	}
}
