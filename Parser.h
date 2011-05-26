#pragma once

#include "foo_musicbrainz.h"
#include "Release.h"

namespace foo_musicbrainz {
	class Parser {
	public:
		static Release *release(const ticpp::Element *);
		static ArtistCredit *artist_credit(const ticpp::Element *);
	};
	
}
