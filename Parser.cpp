#include "foo_musicbrainz.h"
#include "Parser.h"
#include "Date.h"

namespace foo_musicbrainz {
	Release *Parser::release(const ticpp::Element *release_node) {
		auto release = new Release();
		release->set_id(release_node->GetAttribute("id").data());
		auto child = release_node->FirstChildElement(false);
		for (; child; child = child->NextSiblingElement(false)) {
			auto name = child->Value();
			if (name == "title") {
				release->set_title(child->GetText().data());
			} else if (name == "title") {
				release->set_title(child->GetText().data());
			} else if (name == "status") {
				release->set_status(child->GetText().data());
			} else if (name == "date") {
				release->set_date(foo_musicbrainz::Date(child->GetText().data()));
			} else if (name == "country") {
				release->set_country(child->GetText().data());
			} else if (name == "barcode") {
				release->set_barcode(child->GetText().data());
			} else if (name == "asin") {
				release->set_asin(child->GetText().data());
			}
		}

		return release;
	}
}
