#pragma once

#include "foo_musicbrainz.h"
#include "EntityList.h"
#include "NameCredit.h"

namespace foo_musicbrainz {
	class ArtistCredit : public EntityList<NameCredit> {
	public:
		void set_name(const char *new_name) {
			name = new_name;
			internal_name = true;
		}

		pfc::string8 get_name() {
			if (!internal_name) {
				name.reset();
				for (auto i = 0; i < count(); i++) {
					auto name_credit = get(i);
					auto credited_name = name_credit->get_name();
					if (credited_name.is_empty()) {
						name << name_credit->get_artist()->get_name();
					} else {
						name << credited_name;
					}
					name << name_credit->get_joinphrase();
				}
			}
			return name;
		}

		pfc::list_t<pfc::string8> get_ids() {
			pfc::list_t<pfc::string8> ids;
			for (auto i = 0; i < count(); i++) {
				ids.add_item(get(i)->get_artist()->get_id());
			}
			return ids;
		}

		ArtistCredit()
			: EntityList<NameCredit>(),
			internal_name(false) {}

	private:
		pfc::string8 name;
		bool internal_name;
	};
}
