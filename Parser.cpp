#include "foo_musicbrainz.h"
#include "Parser.h"
#include "Date.h"
#include "ArtistCredit.h"
#include "Label.h"
#include "LabelInfo.h"
#include "LabelInfoList.h"
#include "Medium.h"
#include "MediumList.h"
#include "Release.h"
#include "ReleaseGroup.h"
#include "Recording.h"
#include "Track.h"
#include "TrackList.h"

using namespace foo_musicbrainz;

ArtistCredit *Parser::artist_credit(const ticpp::Element *artist_credit_node) {
	auto artist_node = artist_credit_node->FirstChildElement("name-credit")
		->FirstChildElement("artist");

	auto artist_credit = new ArtistCredit();
	artist_credit->set_id(Parser::id(artist_node));

	auto child = artist_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "name") {
			artist_credit->set_name(Parser::text(child));
		}
	}
		
	return artist_credit;
}

Label *Parser::label(const ticpp::Element *label_node) {
	auto label = new Label();

	label->set_id(Parser::id(label_node));

	auto child = label_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "name") {
			label->set_name(Parser::text(child));
		} else if (name == "sort-name") {
			label->set_sort_name(Parser::text(child));
		} else if (name == "label-code") {
			label->set_label_code(Parser::text(child));
		} 
	}

	return label;
}

LabelInfo *Parser::label_info(const ticpp::Element *label_info_node) {
	auto label_info = new LabelInfo();

	auto child = label_info_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "catalog-number") {
			label_info->set_catalog_number(Parser::text(child));
		} else if (name == "label") {
			auto label = Parser::label(child);
			label_info->set_label(label);
		}
	}

	return label_info;
}

LabelInfoList *Parser::label_info_list(const ticpp::Element *label_info_list_node) {
	auto label_info_list = new LabelInfoList();

	auto child = label_info_list_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "label-info") {
			auto label_info = Parser::label_info(child);
			label_info_list->add(label_info);
		}
	}

	return label_info_list;
}

Medium *Parser::medium(const ticpp::Element * medium_node) {
	auto medium = new Medium();

	auto child = medium_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "position") {
			medium->set_position(Parser::integer(child));
		} else if (name == "title") {
			medium->set_title(Parser::text(child));
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

Metadata *Parser::metadata(const ticpp::Element *metadata_node) {
	auto metadata = new Metadata();

	auto child = metadata_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "release") {
			auto release = Parser::release(child);
			metadata->set_release(release);
		} else if (name == "releas-liste") {
			auto release_list = Parser::release_list(child);
			metadata->set_release_list(release_list);
		}
	}

	return metadata;
}

foo_musicbrainz::Release *Parser::release(const ticpp::Element *release_node) {
	auto release = new Release();
	release->set_id(Parser::id(release_node));

	auto child = release_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "title") {
			release->set_title(Parser::text(child));
		} else if (name == "title") {
			release->set_title(Parser::text(child));
		} else if (name == "status") {
			release->set_status(Parser::text(child));
		} else if (name == "date") {
			release->set_date(Date(Parser::text(child)));
		} else if (name == "country") {
			release->set_country(Parser::text(child));
		} else if (name == "barcode") {
			release->set_barcode(Parser::text(child));
		} else if (name == "asin") {
			release->set_asin(Parser::text(child));
		} else if (name == "artist-credit") {
			auto artist_credit = Parser::artist_credit(child);
			release->set_artist_credit(artist_credit);
		} else if (name == "release-group") {
			auto release_group = Parser::release_group(child);
			release->set_release_group(release_group);
		} else if (name == "label-info-list") {
			auto label_info_list = Parser::label_info_list(child);
			release->set_label_info_list(label_info_list);
		} else if (name == "medium-list") {
			auto medium_list = Parser::medium_list(child);
			release->set_medium_list(medium_list);
		}
	}

	return release;
}

ReleaseGroup *Parser::release_group(const ticpp::Element *release_group_node) {
	auto release_group = new ReleaseGroup();
	release_group->set_id(Parser::id(release_group_node));
	release_group->set_type(Parser::text_attr(release_group_node, "type"));

	auto child = release_group_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "title") {
			release_group->set_title(Parser::text(child));
		}
	}

	return release_group;
}

ReleaseList *Parser::release_list(const ticpp::Element *release_list_node) {
	auto release_list = new ReleaseList();

	auto child = release_list_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "release") {
			auto release = Parser::release(child);
			release_list->add(release);
		}
	}

	return release_list;
}

Recording *Parser::recording(const ticpp::Element *recording_node) {
	auto recording = new Recording();
	recording->set_id(Parser::id(recording_node));

	auto child = recording_node->FirstChildElement(false);
	for (; child; child = child->NextSiblingElement(false)) {
		auto name = child->Value();
		if (name == "length") {
			recording->set_length(Parser::integer(child));
		} else if (name == "title") {
			recording->set_title(Parser::text(child));
		} else if (name == "artist-credit") {
			auto artist_credit = Parser::artist_credit(child);
			recording->set_artist_credit(artist_credit);
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
			track->set_position(Parser::integer(child));
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

pfc::string8 Parser::text_attr(const ticpp::Element *element, const char *name) {
	return element->GetAttribute(name).data();
}

pfc::string8 Parser::id(const ticpp::Element *element) {
	return Parser::text_attr(element, "id");
}

pfc::string8 Parser::text(const ticpp::Element *element) {
	return element->GetText(false).data();
}

int Parser::integer(const ticpp::Element *element, int default_value) {
	auto text = Parser::text(element);
	return text.is_empty() ? default_value : atoi(text);
}
