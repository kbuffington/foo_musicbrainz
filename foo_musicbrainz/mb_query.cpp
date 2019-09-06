#include "stdafx.h"
#include "mb_query.h"

mb_query::mb_query(const char* entity, const char* id)
{
	if (mb_preferences::server)
		url << mb_preferences::server_data;
	else
		url << "https://musicbrainz.org";
	url << "/ws/2/" << entity;
	if (id != nullptr) {
		url << "/" << id;
	}
	url << "?fmt=json";
}

char mb_query::to_hex(char c)
{
	return c < 0xa ? '0' + c : 'a' - 0xa + c;
}

void mb_query::add_param(const char* param, const char* value, bool encode)
{
	url << "&" << param << "=" << (encode ? url_encode(value) : value);
}

str8 mb_query::url_encode(const char* in)
{
	str8 out;
	out.prealloc(strlen(in) * 3 + 1);

	for (register const char* tmp = in; *tmp != '\0'; tmp++)
	{
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

json mb_query::lookup(abort_callback& p_abort)
{
	try
	{
		// Download
		auto http = http_client::get();
		auto request = http->create_request("GET");
		request->add_header("User-Agent", "foo_musicbrainz/" COMPONENT_VERSION);
		auto response = request->run_ex(url, p_abort);

		// Get string
		str8 buffer;
		response->read_string_raw(buffer, p_abort);

		return json::parse(buffer.get_ptr());
	}
	catch (const std::exception& e)
	{
		popup_message::g_show(e.what(), COMPONENT_TITLE);
		return json::array();
	}
}
