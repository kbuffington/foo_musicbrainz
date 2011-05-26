#pragma once

#include "foo_musicbrainz.h"
#include "ArtistCredit.h"
#include "Medium.h"
#include "MediumList.h"
#include "Release.h"
#include "ReleaseGroup.h"
#include "Recording.h"
#include "Track.h"
#include "TrackList.h"

namespace foo_musicbrainz {
	class Parser {
	public:
		static ArtistCredit *artist_credit(const ticpp::Element *);
		static Medium *medium(const ticpp::Element *);
		static MediumList *medium_list(const ticpp::Element *);
		static Release *release(const ticpp::Element *);
		static ReleaseGroup *release_group(const ticpp::Element *);
		static Recording *recording(const ticpp::Element *);
		static Track *track(const ticpp::Element *);
		static TrackList *track_list(const ticpp::Element *);
	};
	
}
