#include "foo_musicbrainz.h"
#include "Parser.h"
#include "ArtistCredit.h"
#include "Release.h"
#include "Date.h"

using namespace foo_musicbrainz;

foo_musicbrainz::Release *Parser::release(const ticpp::Element *release_node) {
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
			release->set_date(Date(child->GetText().data()));
		} else if (name == "country") {
			release->set_country(child->GetText().data());
		} else if (name == "barcode") {
			release->set_barcode(child->GetText().data());
		} else if (name == "asin") {
			release->set_asin(child->GetText().data());
		} else if (name == "artist-credit") {
			auto artist_credit = Parser::artist_credit(child);
			release->set_artist_credit(artist_credit);
		}
	}

	return release;
}

ArtistCredit *Parser::artist_credit(const ticpp::Element *artist_credit_node) {
	auto artist_node = artist_credit_node->FirstChildElement("name-credit")
		->FirstChildElement("artist");

	auto artist_credit = new ArtistCredit();
	artist_credit->set_id(artist_node->GetAttribute("id").data());

	auto child = artist_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "name") {
			artist_credit->set_name(child->GetText().data());
		}
	}
		
	return artist_credit;
}
