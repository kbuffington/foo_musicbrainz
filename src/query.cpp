#include "stdafx.h"
#include "query.h"

namespace mb
{
	query::query(pfc::stringp entity, pfc::stringp id)
	{
		url << prefs::get_server() << "/ws/2/" << entity;
		if (id.length()) url << "/" << id;
		url << "?fmt=json";
	}

	json query::lookup(abort_callback& p_abort)
	{
		try
		{
			// Download
			auto http = http_client::get();
			auto request = http->create_request("GET");
			request->add_header("User-Agent", PFC_string_formatter() << "foo_musicbrainz/" << component_version);
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

			popup_message::g_show(buffer, component_title);
			return json();
		}
		catch (const std::exception& e)
		{
			popup_message::g_show(e.what(), component_title);
			return json();
		}
	}

	void query::add_param(pfc::stringp param, pfc::stringp value)
	{
		url << "&" << param << "=" << value;
	}
}
