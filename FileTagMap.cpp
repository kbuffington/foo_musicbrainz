#include "foo_musicbrainz.h"
#include "FileTagMap.h"
#include "Medium.h"
#include "Release.h"
#include "Track.h"

using namespace foo_musicbrainz;

Tag::Tag(Release &release, Medium &medium, Track &track) {
	set("ALBUM", release.get_title());
	pfc::string8 date = release.get_date();
	if (!date.is_empty()) {
		set("DATE", date);
	}

	set("TITLE", track.get_title());
	set("TRACKNUMBER", track.get_position());
	set("TOTALTRACKS", medium.track_count());

	if (auto medium_count = release.medium_count() > 1) {
		set("DISCNUMBER", medium.get_position());
		set("TOTALDISCS", medium_count);
		set("DISCSUBTITLE", medium.get_title());
	}

	// Album type
	if (Preferences::albumtype) {
		auto type = release.get_release_group()->get_type();
		if (type != ReleaseGroup::types[0]) {
			set(Preferences::albumstatus_data, type);
		} else {
			set(Preferences::albumstatus_data, "");
		}
	}

	// Album status
	if (Preferences::albumstatus) {
		auto status = release.get_status();
		if (status != Release::statuses[0]) {
			set(Preferences::albumstatus_data, status);
		} else {
			set(Preferences::albumstatus_data, "");
		}
	}

	// Artist
	bool va = release.is_various();
	set("ARTIST", track.get_artist_credit()->get_name());
	set("ALBUM ARTIST", va ? release.get_artist_credit()->get_name() : "");

	// MusicBrainz IDs
	if (Preferences::write_ids) {
		set("MUSICBRAINZ_ALBUMID", release.get_id());
		set("MUSICBRAINZ_RELEASEGROUPID", release.get_release_group()->get_id());
		set("MUSICBRAINZ_TRACKID", track.get_id());
		base_class::set("MUSICBRAINZ_ARTISTID", track.get_artist_credit()->get_ids());
		if (va) {
			base_class::set("MUSICBRAINZ_ALBUMARTISTID", release.get_artist_credit()->get_ids());
		} else {
			set("MUSICBRAINZ_ALBUMARTISTID", "");
		}
		// TODO: disc id
		//if (strcmp(medium->get_di->getDiscId(), "") != 0) {
		//	set("MUSICBRAINZ_DISCID", mbc->getDiscId());
		//}
	}

	// Barcode
	set("BARCODE", release.get_barcode());

	// Label info
	TagValues labels, catalog_numbers;
	for (auto i = 0; i < release.label_info_count(); i++) {
		// TODO: possibly remove duplicates?
		if (auto label = release.get_label_info(i)->get_label()->get_name()) {
			labels.add_item(label);
		}
		if (auto catalog_number = release.get_label_info(i)->get_catalog_number()) {
			catalog_numbers.add_item(catalog_number);
		}
	}
	base_class::set("LABEL", labels);
	base_class::set("CATALOGNUMBER", catalog_numbers);
}

void Tag::set(pfc::string8 key, pfc::string8 value) {
	TagValues list;
	list.add_item(value);
	base_class::set(key, list);
}

void Tag::set(pfc::string8 key, const char *value) {
	set(key, pfc::string8(value));
}

void Tag::set(pfc::string8 key, int value) {
	pfc::string8 tmp;
	tmp << value;
	set(key, tmp);
}

FileTagMap::FileTagMap(Release &release, pfc::list_t<metadb_handle_ptr> tracks) {
	auto current_medium = 0;
	auto current_track = 0;
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
