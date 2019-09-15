#include "stdafx.h"
#include "mb_query.h"

mb_query::mb_query(const char* entity, const char* id)
{
	url << get_server() << "/ws/2/" << entity;
	if (id != nullptr) url << "/" << id;
	url << "?fmt=json";
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

void mb_query::add_param(const char* param, const char* value)
{
	url << "&" << param << "=" << value;
}
