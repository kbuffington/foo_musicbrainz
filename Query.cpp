#include "foo_musicbrainz.h"
#include <Winhttp.h>
#include "Query.h"
#include "Metadata.h"
#include "Parser.h"

using namespace foo_musicbrainz;

Query::Query(const char *entity, const char *id) {
	url << "/ws/2/" << entity;
	if (id != nullptr) {
		url << "/" << id;
	}
	url << "?";
}

#define TO_HEX(c) (c < 0xa ? '0' + c : 'a' - 0xa + c)

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
		if (isalnum(*tmp)) {
			out.add_char(*tmp);
		} else if (isspace(*tmp)) {
			out.add_char('+');
		} else {
			out.add_char('%');
			out.add_char(TO_HEX(((unsigned char)*tmp)>>4));
			out.add_char(TO_HEX(((unsigned char)*tmp)%16));
		}
	}
	// out.add_char('\0'); really needed?
	return out;
}

void Query::get(pfc::string8 &buffer) {
#ifdef DEBUG
	console::printf("MusicBrainz tagger: accessing http://musicbrainz.org%s", url.get_ptr());
#endif
	pfc::stringcvt::string_os_from_utf8 url_wide(url);
	HINTERNET hSession, hConnect, hRequest;

	try {
		hSession = WinHttpOpen(L"foo_musicbrainz/" _T(COMPONENT_VERSION), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession == NULL) throw ConnectionError();
		hConnect = WinHttpConnect(hSession, L"musicbrainz.org", INTERNET_DEFAULT_HTTP_PORT, 0);
		if (hConnect == NULL) throw ConnectionError();
		hRequest = WinHttpOpenRequest(hConnect, L"GET", url_wide, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		if (hRequest == NULL) throw ConnectionError();
		if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) throw ConnectionError();
		unsigned long size;
		if (!WinHttpReceiveResponse(hRequest, NULL) || !WinHttpQueryDataAvailable(hRequest, &size)) throw ConnectionError();
		while (size > 0) {
			char *tmp = new char[size];
			unsigned long downloaded;
			WinHttpReadData(hRequest, tmp, size, &downloaded);
			buffer.add_string(tmp, downloaded);
			delete [] tmp;
			WinHttpQueryDataAvailable(hRequest, &size);
		};
	} catch (ConnectionError e) {
		if (hRequest != nullptr) WinHttpCloseHandle(hRequest);
		if (hConnect != nullptr) WinHttpCloseHandle(hConnect);
		if (hSession != nullptr) WinHttpCloseHandle(hSession);
		throw e;
	}

	if (hRequest != nullptr) WinHttpCloseHandle(hRequest);
	if (hConnect != nullptr) WinHttpCloseHandle(hConnect);
	if (hSession != nullptr) WinHttpCloseHandle(hSession);
}

ticpp::Element *Query::parse(pfc::string8 &buffer, ticpp::Document &xml) {
	// Parsing XML
	try {
		xml.Parse(buffer.get_ptr(), true, TIXML_ENCODING_UTF8);
	} catch (ticpp::Exception) {
#ifdef DEBUG
		pfc::string8 error = "Error parsing XML, response from musicbrainz.org:\n\n";
		error << buffer;
		throw XmlParseError(error);
#else
		throw XmlParseError();
#endif
	}

	// Accessing metadata element
	try {
		return xml.FirstChildElement("metadata");
	} catch (ticpp::Exception) {
		try {
			auto text = xml.FirstChildElement("error")->FirstChildElement("text");
			pfc::string8 message = "Response from MusicBrainz: ";
			message << Parser::text(text);
			// FIXME: not only not found (404), but also 401 and 404
			throw NotFound(message);
		} catch (ticpp::Exception) {
			throw XmlParseError();
		}
	}
}

Metadata *Query::perform() {
	pfc::string8 buffer;
	get(buffer);
	ticpp::Document xml;
	return Parser::metadata(parse(buffer, xml));
}
