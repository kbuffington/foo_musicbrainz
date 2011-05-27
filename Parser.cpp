#include "foo_musicbrainz.h"
#include "Parser.h"
#include "Date.h"
#include "ArtistCredit.h"
#include "Medium.h"
#include "MediumList.h"
#include "Release.h"
#include "ReleaseGroup.h"
#include "Track.h"
#include "TrackList.h"

using namespace foo_musicbrainz;

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

Medium *Parser::medium(const ticpp::Element * medium_node) {
	auto medium = new Medium();

	auto child = medium_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "position") {
			auto position = atoi(child->GetText().data());
			medium->set_position(position);
		} else if (name == "title") {
			medium->set_title(child->GetText().data());
		} else if (name == "track-list") {
			auto track_list = Parser::track_list(child);
			medium->set_track_list(track_list);
		}
	}

	return medium;
}

MediumList *Parser::medium_list(const ticpp::Element *medium_list_node) {
	auto medium_list = new MediumList();

	auto child = medium_list_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "medium") {
			auto medium = Parser::medium(child);
			medium_list->add(medium);
		}
	}

	return medium_list;
}

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
		} else if (name == "release-group") {
			auto release_group = Parser::release_group(child);
			release->set_release_group(release_group);
		} else if (name == "medium-list") {
			auto medium_list = Parser::medium_list(child);
			release->set_medium_list(medium_list);
		}
	}

	return release;
}

ReleaseGroup *Parser::release_group(const ticpp::Element *release_group_node) {
	auto release_group = new ReleaseGroup();
	release_group->set_id(release_group_node->GetAttribute("id").data());
	release_group->set_type(release_group_node->GetAttribute("type").data());

	auto child = release_group_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "title") {
			release_group->set_title(child->GetText().data());
		}
	}

	return release_group;
}

Recording *Parser::recording(const ticpp::Element *recording_node) {
	auto recording = new Recording();
	recording->set_id(recording_node->GetAttribute("id").data());

	auto child = recording_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "length") {
			auto length = atoi(child->GetText().data());
			recording->set_length(length);
		} else if (name == "title") {
			recording->set_title(child->GetText().data());
		}
	}

	return recording;
}

Track *Parser::track(const ticpp::Element *track_node) {
	auto track = new Track();

	auto child = track_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "position") {
			auto position = atoi(child->GetText().data());
			track->set_position(position);
		} else if (name == "recording") {
			auto recording = Parser::recording(child);
			track->set_recording(recording);
		}
	}

	return track;
}

TrackList *Parser::track_list(const ticpp::Element *track_list_node) {
	auto track_list = new TrackList();

	auto child = track_list_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "track") {
			auto track = Parser::track(child);
			track_list->add(track);
		}
	}

	return track_list;
}
