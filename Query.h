#pragma once

#include "foo_musicbrainz.h"
#include "Metadata.h"

namespace foo_musicbrainz {
	class Query {
	public:
		Query(pfc::string8 id, pfc::string8 &includes = pfc::string8());
		void add_param(const char *param, const char *value, bool encode);
		void add_param(const char *param, int value, bool encode = true);
		Metadata *perform();

	private:
		pfc::string8 url_encode(const char *);
		void get(pfc::string8 &buffer);
		ticpp::Element *parse(pfc::string8 &buffer, ticpp::Document &xml);

		pfc::string8 url;
	};
}
