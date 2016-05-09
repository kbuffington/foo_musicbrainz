#include "foo_musicbrainz.h"
#include "Query.h"
#include "Metadata.h"
#include "Parser.h"

using namespace foo_musicbrainz;

Query::Query(const char *entity, const char *id) {
	url << "http://musicbrainz.org/ws/2/" << entity;
	if (id != nullptr) {
		url << "/" << id;
	}
	url << "?";
}

char Query::to_hex(char c) {
	return c < 0xa ? '0' + c : 'a' - 0xa + c;
}

void Query::add_param(const char *param, const char *value, bool encode) {
	url << param << "=" << (encode ? url_encode(value) : value) << "&";
}

void Query::add_param(const char *param, int value) {
	pfc::string8 str;
	str << value;
	add_param(param, str, false);
}

pfc::string8 Query::url_encode(const char *in) {
	pfc::string8 out;
	out.prealloc(strlen(in) * 3 + 1);

	for (register const char *tmp = in; *tmp != '\0'; tmp++) {
		auto c = static_cast<unsigned char>(*tmp);
		if (isalnum(c)) {
			out.add_char(c);
		} else if (isspace(c)) {
			out.add_char('+');
		} else {
			out.add_char('%');
			out.add_char(to_hex(c >> 4));
			out.add_char(to_hex(c % 16));
		}
	}
	return out;
}

TiXmlElement *Query::parse(pfc::string8 &buffer, TiXmlDocument &xml) {
	// Parsing XML
	xml.Parse(buffer, 0, TIXML_ENCODING_UTF8);
	if (xml.Error()) {
#ifdef DEBUG
		pfc::string8 error = "Error parsing XML, response from musicbrainz.org:\n\n";
		error << buffer;
		throw XmlParseError(error);
#else
		throw XmlParseError();
#endif
	}

	// Accessing <metadata> element
	auto metadata = xml.FirstChildElement("metadata");
	if (metadata != nullptr) return metadata;

	// No metadata, look for <error> element
	auto error = xml.FirstChildElement("error");
	if (error == nullptr) throw XmlParseError();
	auto text = error->FirstChildElement("text");
	if (text == nullptr) throw XmlParseError();
	pfc::string8 message = "Response from MusicBrainz: ";
	message << Parser::text(text);
	// FIXME: not only not found (404), but also 401 and 404
	throw NotFound(message);
}

Metadata *Query::perform(abort_callback &callback) {
#ifdef DEBUG
	auto logger = uDebugLog();
	logger << "MusicBrainz tagger: accessing " << url;
#endif

	// Download
	static_api_ptr_t<http_client> http;
	auto request = http->create_request("GET");
	request->add_header("User-Agent", "foo_musicbrainz/" COMPONENT_VERSION);
	auto response = request->run_ex(url, callback);
	
	// Get string
	pfc::string8 buffer;
	response->read_string_raw(buffer, callback);
	
	// Parse
	TiXmlDocument xml;
	return Parser::metadata(parse(buffer, xml));
}
