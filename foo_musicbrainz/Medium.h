#pragma once

#include "foo_musicbrainz.h"
#include "Track.h"

namespace foo_musicbrainz {
	class Medium {
		STRING_MEMBER(format)
		MEMBER(int, position)
		STRING_MEMBER(title)
		STRING_MEMBER(discid)
		COLLECTION(Track, track)

	public:
		pfc::string8 get_info() {
			pfc::string8 info;
			info << "Disc " << position;
			if (title.get_length()) {
				info << ": " << title;
			}
			auto track_count = track_items.get_count();
			info << " (" << track_count << " track";
			if (track_count > 1) {
				info << "s";
			}
			info << ")";
			return info;
		}

		Medium()
			: position(0) {};
		~Medium() {
			COLLECTION_DESTRUCTOR(track)
		}
	};
}
