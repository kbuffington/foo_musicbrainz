#include "foo_musicbrainz.h"
#include "Parser.h"
#include "Date.h"
#include "Artist.h"
#include "ArtistCredit.h"
#include "DiscID.h"
#include "Label.h"
#include "LabelInfo.h"
#include "LabelInfoList.h"
#include "Medium.h"
#include "MediumList.h"
#include "NameCredit.h"
#include "Release.h"
#include "ReleaseGroup.h"
#include "Recording.h"
#include "Track.h"
#include "TrackList.h"

using namespace foo_musicbrainz;

pfc::string8 Parser::text_attr(const TiXmlElement *element, const char *name) {
	auto value =  element->Attribute(name);
	return value == nullptr ? "" : value;
}

pfc::string8 Parser::id(const TiXmlElement *element) {
	return Parser::text_attr(element, "id");
}

pfc::string8 Parser::text(const TiXmlElement *element) {
	auto value =  element->GetText();
	return value == nullptr ? "" : value;
}

Date Parser::date(const TiXmlElement *element) {
	return Date(Parser::text(element));
}

int Parser::integer(const TiXmlElement *element, int default_value) {
	auto value =  element->GetText();
	return value == nullptr ? default_value : atoi(value);
}

// Defines new method for parsing an element node.
// @param class_name Class which should be used to construct entity.
// @param method_name This name will be given to the generated method. 
// @param children Place where to put children and attribute parsing.
#define ELEMENT(class_name, method_name, children, attributes) \
	class_name *Parser::method_name(const TiXmlElement *node) { \
		auto entity = new class_name(); \
		auto child = node->FirstChildElement(); \
		for (; child; child = child->NextSiblingElement()) { \
			auto name = child->Value(); \
			if (false) {} \
			children \
		} \
		attributes \
		return entity; \
	}

// Parses a child element.
// @param tag_name Tag name used to identity a child of this kind. Usually dasherized.
// @param add_method How to add to the parent. Usually underscored.
// @param parser_method How to parse. For text nodes this is usually text or integer methods,
// for element nodes it matches underscored name of en entity.
#define PARSE_CHILD(tag_name, add_method, parser_method) \
	else if (strcmp(name, #tag_name) == 0) { \
		entity->add_method(Parser::parser_method(child)); \
	}

// Parses a child which can be the only one of its kind for his parent.
// @param tag_name Same as above.
// @param entity_name Underscored name of the resulting entity.
// @param parser_method Same as above.
#define PARSE_SINGLE_CHILD(tag_name, entity_name, parser_method) \
	PARSE_CHILD(tag_name, set_##entity_name, parser_method)

// Parses a child as a part of homogenous collection of its parent.
// @param tag_name Same as above.
// @param parser_method Same as above.
#define PARSE_CHILDREN(tag_name, parser_method) \
	PARSE_CHILD(tag_name, add, parser_method)

// Parses an attribute.
// @param attr_name Attribute name.
// @param entity_name Underscored name of the resulting entity.
// @param parser_method Same as above.
#define PARSE_ATTRIBUTE(attr_name, entity_name, parser_method) \
	entity->set_##entity_name(Parser::parser_method##_attr(node, #attr_name));

// List of all node types. Relates node name to the parser method and method used for adding entity to the parent. 
#define ARTIST PARSE_SINGLE_CHILD(artist, artist, artist)
#define ARTIST_CREDIT PARSE_SINGLE_CHILD(artist-credit, artist_credit, artist_credit)
#define ASIN PARSE_SINGLE_CHILD(asin, asin, text)
#define BARCODE PARSE_SINGLE_CHILD(barcode, barcode, text)
#define CATALOG_NUMBER PARSE_SINGLE_CHILD(catalog-number, catalog_number, text)
#define COUNTRY PARSE_SINGLE_CHILD(country, country, text)
#define DATE PARSE_SINGLE_CHILD(date, date, date)
#define DISCID_SINGLE PARSE_SINGLE_CHILD(disc, discid, discid)
#define ID PARSE_ATTRIBUTE(id, id, text)
#define JOINPHRASE PARSE_ATTRIBUTE(joinphrase, joinphrase, text)
#define NAME PARSE_SINGLE_CHILD(name, name, text)
#define LABEL PARSE_SINGLE_CHILD(label, label, label)
#define LABEL_CODE PARSE_SINGLE_CHILD(label-code, label_code, text)
#define LABEL_INFO PARSE_CHILDREN(label-info, label_info)
#define LABEL_INFO_LIST PARSE_SINGLE_CHILD(label-info-list, label_info_list, label_info_list)
#define LENGTH PARSE_SINGLE_CHILD(length, length, integer)
#define MEDIUM PARSE_CHILDREN(medium, medium)
#define MEDIUM_LIST PARSE_SINGLE_CHILD(medium-list, medium_list, medium_list)
#define NAME PARSE_SINGLE_CHILD(name, name, text)
#define NAME_CREDIT PARSE_CHILDREN(name-credit, name_credit)
#define POSITION PARSE_SINGLE_CHILD(position, position, integer)
#define RECORDING PARSE_SINGLE_CHILD(recording, recording, recording)
#define RELEASE PARSE_CHILDREN(release, release)
#define RELEASE_LIST PARSE_SINGLE_CHILD(release-list, release_list, release_list)
#define RELEASE_GROUP PARSE_SINGLE_CHILD(release-group, release_group, release_group)
#define RELEASE_SINGLE PARSE_SINGLE_CHILD(release, release, release)
#define SORT_NAME PARSE_SINGLE_CHILD(sort-name, sort_name, text)
#define SECTORS PARSE_SINGLE_CHILD(sectors, sectors, integer)
#define STATUS PARSE_SINGLE_CHILD(status, status, text)
#define TITLE PARSE_SINGLE_CHILD(title, title, text)
#define TRACK PARSE_CHILDREN(track, track)
#define TYPE PARSE_ATTRIBUTE(type, type, text)
#define TRACK_LIST PARSE_SINGLE_CHILD(track-list, track_list, track_list)

// List of all element node types. Relates node class to the list of child element types by generating a parser method.
ELEMENT(Artist, artist,
	NAME SORT_NAME,
	ID
)
ELEMENT(ArtistCredit, artist_credit,
	NAME_CREDIT
)
ELEMENT(DiscID, discid,
	RELEASE_LIST SECTORS,
	ID
)
ELEMENT(Label, label,
	NAME LABEL_CODE SORT_NAME,
	ID
)
ELEMENT(LabelInfo, label_info,
	CATALOG_NUMBER LABEL
)
ELEMENT(LabelInfoList, label_info_list,
	LABEL_INFO
)
ELEMENT(Medium, medium,
	POSITION TITLE TRACK_LIST
)
ELEMENT(MediumList, medium_list,
	MEDIUM
)
ELEMENT(Metadata, metadata,
	DISCID_SINGLE RELEASE_LIST RELEASE_SINGLE
)
ELEMENT(NameCredit, name_credit,
	ARTIST NAME,
	JOINPHRASE
)
ELEMENT(Release, release,
	ARTIST_CREDIT ASIN BARCODE COUNTRY DATE LABEL_INFO_LIST MEDIUM_LIST RELEASE_GROUP STATUS TITLE,
	ID
)
ELEMENT(ReleaseGroup, release_group,
	TITLE,
	ID TYPE
)
ELEMENT(ReleaseList, release_list,
	RELEASE
)
ELEMENT(Recording, recording,
	ARTIST_CREDIT LENGTH TITLE,
	ID
)
ELEMENT(Track, track,
	POSITION RECORDING
)
ELEMENT(TrackList, track_list,
	TRACK
)
