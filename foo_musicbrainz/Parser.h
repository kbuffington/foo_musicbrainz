#pragma once

#include "foo_musicbrainz.h"
#include "ArtistCredit.h"
#include "Date.h"
#include "Artist.h"
#include "DiscID.h"
#include "LabelInfo.h"
#include "Medium.h"
#include "Metadata.h"
#include "NameCredit.h"
#include "Release.h"
#include "ReleaseGroup.h"
#include "ReleaseList.h"
#include "Track.h"

namespace foo_musicbrainz {
	class Parser {
	public:
		static Artist *artist(const TiXmlElement *);
		static ArtistCredit *artist_credit(const TiXmlElement *);
		static DiscID *discid(const TiXmlElement *);
		static LabelInfo *label_info(const TiXmlElement *);
		static Medium *medium(const TiXmlElement *);
		static NameCredit *name_credit(const TiXmlElement *);
		static Metadata *metadata(const TiXmlElement *);
		static Release *release(const TiXmlElement *);
		static ReleaseGroup *release_group(const TiXmlElement *);
		static ReleaseList *release_list(const TiXmlElement *);
		static Track *track(const TiXmlElement *);

		static pfc::string8 text_attr(const TiXmlElement *, const char *);
		static pfc::string8 id(const TiXmlElement *);
		static pfc::string8 text(const TiXmlElement *);
		static Date date(const TiXmlElement *);
		static int integer(const TiXmlElement *, int default_value = 0);
		static int integer_attr(const TiXmlElement *, const char *, int default_value = 0);
	};
}
