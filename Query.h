#pragma once

#include "foo_musicbrainz.h"
#include "Metadata.h"

namespace foo_musicbrainz {
	class Query {
	public:
		Query(const char *entity, const char *id = "");
		void add_param(const char *param, const char *value, bool encode = true);
		void add_param(const char *param, int value);
		Metadata *perform(abort_callback &callback = abort_callback_dummy());

	private:
		inline char to_hex(char);
		pfc::string8 url_encode(const char *);
		ticpp::Element *parse(pfc::string8 &buffer, ticpp::Document &xml);

		pfc::string8 url;
	};
}
