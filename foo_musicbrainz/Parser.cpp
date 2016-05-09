#include "foo_musicbrainz.h"
#include "Parser.h"
#include "Date.h"
#include "Artist.h"
#include "ArtistCredit.h"
#include "DiscID.h"
#include "LabelInfo.h"
#include "Medium.h"
#include "NameCredit.h"
#include "Release.h"
#include "ReleaseGroup.h"
#include "Track.h"

using namespace foo_musicbrainz;

pfc::string8 Parser::text_attr(const TiXmlElement *element, const char *name) {
	auto value = element->Attribute(name);
	return value == nullptr ? "" : value;
}

pfc::string8 Parser::id(const TiXmlElement *element) {
	return Parser::text_attr(element, "id");
}

pfc::string8 Parser::text(const TiXmlElement *element) {
	auto value = element->GetText();
	return value == nullptr ? "" : value;
}

Date Parser::date(const TiXmlElement *element) {
	return Date(Parser::text(element));
}

int Parser::integer(const TiXmlElement *element, int default_value) {
	auto value = element->GetText();
	return value == nullptr ? default_value : atoi(value);
}

int Parser::integer_attr(const TiXmlElement *element, const char *name, int default_value) {
	auto value = element->Attribute(name);
	return value == nullptr ? default_value : atoi(value);
}

// Loop for parsing children and attributes.
// @param method_name This name will be given to the generated method. 
// @param children Place where to put children and attribute parsing.
#define CHILDER(children, attributes) \
	{ \
		auto child = node->FirstChildElement(); \
		for (; child; child = child->NextSiblingElement()) { \
			auto name = child->Value(); \
			if (false) {} \
			children \
		} \
		attributes \
	}

// Defines new method for parsing an element node.
// @param class_name Class which should be used to construct entity.
// @param children Same as above.
// @param attributes Same as above.
#define ELEMENT(class_name, method_name, children, attributes) \
	class_name *Parser::method_name(const TiXmlElement *node) { \
		auto entity = new class_name(); \
		CHILDER(children, attributes) \
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

// Parses a child as a part of heterogeneous collection of its parent
// (or, to be more correct, several homogenous collections).
// @param tag_name Same as above.
// @param entity_name Underscored name of the resulting entity, used to
// specify the collection child will be added to.
// @param parser_method Same as above.
#define PARSE_CHILDREN_MIXED(tag_name, entity_name, parser_method) \
	PARSE_CHILD(tag_name, add_##entity_name, parser_method)

// Parses an attribute.
// @param attr_name Attribute name.
// @param entity_name Underscored name of the resulting entity.
// @param parser_method Same as above.
#define PARSE_ATTRIBUTE(attr_name, entity_name, parser_method) \
	entity->set_##entity_name(Parser::parser_method##_attr(node, #attr_name));

// Works like ELEMENT, but instead of defining a new method, should be placed
// inside a child loop, thus allowing parent to contain its grandchildren.
// @param tag_name Same as above.
// @param children Same as above.
// @param attributes Same as above.
#define INLINE(tag_name, children, attributes) \
	else if (strcmp(name, #tag_name) == 0) { \
		auto node = child; \
		CHILDER(children, attributes) \
	}

// List of all node types. Relates node name to the parser method and method used for adding entity to the parent. 
#define ARTIST PARSE_SINGLE_CHILD(artist, artist, artist)
#define ARTIST_CREDIT PARSE_SINGLE_CHILD(artist-credit, artist_credit, artist_credit)
#define ASIN PARSE_SINGLE_CHILD(asin, asin, text)
#define BARCODE PARSE_SINGLE_CHILD(barcode, barcode, text)
#define CATALOG_NUMBER PARSE_SINGLE_CHILD(catalog-number, catalog_number, text)
#define COUNTRY PARSE_SINGLE_CHILD(country, country, text)
#define DATE PARSE_SINGLE_CHILD(date, date, date)
#define DISCID_SINGLE PARSE_SINGLE_CHILD(disc, discid, discid)
#define FIRST_RELEASE_DATE PARSE_SINGLE_CHILD(first-release-date, first_release_date, date)
#define ID PARSE_ATTRIBUTE(id, id, text)
#define JOINPHRASE PARSE_ATTRIBUTE(joinphrase, joinphrase, text)
#define NAME PARSE_SINGLE_CHILD(name, name, text)
#define LABEL(children, attributes) INLINE(label, children, attributes)
#define LABEL_CODE PARSE_SINGLE_CHILD(label-code, label_code, text)
#define LABEL_INFO PARSE_CHILDREN_MIXED(label-info, label_info, label_info)
#define LABEL_INFO_LIST(children, attributes) INLINE(label-info-list, children, attributes)
#define LANGUAGE PARSE_SINGLE_CHILD(language, language, text)
#define LENGTH PARSE_SINGLE_CHILD(length, length, integer)
#define MEDIUM PARSE_CHILDREN_MIXED(medium, medium, medium)
#define MEDIUM_COUNT PARSE_ATTRIBUTE(count, medium_total_count, integer)
#define MEDIUM_LIST(children, attributes) INLINE(medium-list, children, attributes)
#define NAME PARSE_SINGLE_CHILD(name, name, text)
#define NAME_CREDIT PARSE_CHILDREN(name-credit, name_credit)
#define POSITION PARSE_SINGLE_CHILD(position, position, integer)
#define RECORDING(children, attributes) INLINE(recording, children, attributes)
#define RELEASE PARSE_CHILDREN(release, release)
#define RELEASE_LIST PARSE_SINGLE_CHILD(release-list, release_list, release_list)
#define RELEASE_GROUP PARSE_SINGLE_CHILD(release-group, release_group, release_group)
#define RELEASE_SINGLE PARSE_SINGLE_CHILD(release, release, release)
#define SCRIPT PARSE_SINGLE_CHILD(script, script, text)
#define SECTORS PARSE_SINGLE_CHILD(sectors, sectors, integer)
#define SORT_NAME PARSE_SINGLE_CHILD(sort-name, sort_name, text)
#define STATUS PARSE_SINGLE_CHILD(status, status, text)
#define TEXT_REPRESENTATION(children, attributes) INLINE(text-representation, children, attributes)
#define TITLE PARSE_SINGLE_CHILD(title, title, text)
#define TRACK PARSE_CHILDREN_MIXED(track, track, track)
#define TYPE PARSE_ATTRIBUTE(type, type, text)
#define TRACK_LIST(children, attributes) INLINE(track-list, children, attributes)

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
ELEMENT(LabelInfo, label_info,
	CATALOG_NUMBER LABEL(
		NAME LABEL_CODE SORT_NAME,
		ID
	)
)
ELEMENT(Medium, medium,
	POSITION TITLE TRACK_LIST(
		TRACK
	)
)
ELEMENT(Metadata, metadata,
	DISCID_SINGLE RELEASE_LIST RELEASE_SINGLE
)
ELEMENT(NameCredit, name_credit,
	ARTIST NAME,
	JOINPHRASE
)
ELEMENT(Release, release,
	ARTIST_CREDIT ASIN BARCODE COUNTRY DATE RELEASE_GROUP STATUS TITLE
	LABEL_INFO_LIST(
		LABEL_INFO
	)
	MEDIUM_LIST(
		MEDIUM,
		MEDIUM_COUNT
	)
	TEXT_REPRESENTATION(
		LANGUAGE SCRIPT
	),
	ID
)
ELEMENT(ReleaseGroup, release_group,
	TITLE FIRST_RELEASE_DATE,
	ID TYPE
)
ELEMENT(ReleaseList, release_list,
	RELEASE
)
ELEMENT(Track, track,
	POSITION RECORDING(
		ARTIST_CREDIT LENGTH TITLE,
		ID
	)
)
