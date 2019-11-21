#include "stdafx.h"
#include "query.h"

namespace mb
{
	query::query(const char* entity, const char* id)
	{
		url << prefs::get_server() << "/ws/2/" << entity;
		if (id != nullptr) url << "/" << id;
		url << "?fmt=json";
	}

	json query::lookup(abort_callback& p_abort)
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

			json j = json::parse(buffer.get_ptr(), nullptr, false);
			if (j.is_object())
			{
				return j;
			}

			http_reply::ptr ptr;
			if (response->cast(ptr))
			{
				ptr->get_status(buffer);
			}

			popup_message::g_show(buffer, COMPONENT_TITLE);
			return json();
		}
		catch (const std::exception& e)
		{
			popup_message::g_show(e.what(), COMPONENT_TITLE);
			return json();
		}
	}

	void query::add_param(const char* param, const char* value)
	{
		url << "&" << param << "=" << value;
	}
}
