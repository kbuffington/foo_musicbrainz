#pragma once

#include "foo_musicbrainz.h"
#include "ArtistCredit.h"
#include "Label.h"
#include "LabelInfo.h"
#include "LabelInfoList.h"
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
		static Label *label(const ticpp::Element *);
		static LabelInfo *label_info(const ticpp::Element *);
		static LabelInfoList *label_info_list(const ticpp::Element *);
		static Medium *medium(const ticpp::Element *);
		static MediumList *medium_list(const ticpp::Element *);
		static Release *release(const ticpp::Element *);
		static ReleaseGroup *release_group(const ticpp::Element *);
		static Recording *recording(const ticpp::Element *);
		static Track *track(const ticpp::Element *);
		static TrackList *track_list(const ticpp::Element *);

		static pfc::string8 text_attr(const ticpp::Element *, const char *);
		static pfc::string8 id(const ticpp::Element *);
		static pfc::string8 text(const ticpp::Element *);
		static int integer(const ticpp::Element *, int default_value = 0);
	};
	
}
