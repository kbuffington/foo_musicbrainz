#pragma once

#include "foo_musicbrainz.h"
#include "ReleaseList.h"
#include "Release.h"
#include "Medium.h"
#include "Track.h"

namespace foo_musicbrainz {
	class MetadataProcessor : public service_base {
	public:
		enum Entities {
			none                = 0,
			release_list_entity = 1,
			release_entity      = 1 << 1,
			medium_entity       = 1 << 2,
			track_entity        = 1 << 3
		};
		
		virtual unsigned int get_entities() = 0;
		virtual bool is_enabled() { return true; }
		virtual void process(ReleaseList &) {};
		virtual void process(Release &) {};
		virtual void process(Medium &) {};
		virtual void process(Track &) {};
		
		static void apply_all(ReleaseList &release_list);
		static void apply_one(service_ptr_t<MetadataProcessor>, ReleaseList &);
 
		FB2K_MAKE_SERVICE_INTERFACE_ENTRYPOINT(MetadataProcessor);
	};
}

