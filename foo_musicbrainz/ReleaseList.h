#pragma once

#include "foo_musicbrainz.h"
#include "EntityList.h"
#include "Release.h"

namespace foo_musicbrainz {
	class ReleaseList : public EntityList<Release> {

	public:
		void sort() {
			this->items.sort(release_sort_callback());
		}

	private:
		class release_sort_callback : public pfc::list_base_t<Release *>::sort_callback {
		public:
			release_sort_callback() {}
			virtual int compare(Release *const& left, Release *const& right) {
				return Date::compare(left->get_date(), right->get_date());
			}
		};
	};
}
