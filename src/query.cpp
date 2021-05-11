#include "stdafx.h"
#include "query.h"
#include "query_cache.h"
#include <mutex>

namespace mb
{
	query_cache cache;
	uint64_t last_http_request = 0;
	std::mutex querying_mb;

	query::query(pfc::stringp entity, pfc::stringp id)
	{
		url << prefs::get_server() << "/ws/2/" << entity;
		if (id.length()) url << "/" << id;
		url << "?fmt=json";
	}

	json query::lookup(abort_callback& abort)
	{
		try
		{
			str8 buffer;
			if (cache.get(url, &buffer)) {
				json j = json::parse(buffer.get_ptr(), nullptr, false);
				if (j.is_object())
				{
					return j;
				}
			}
			// cache miss

			// Download
			auto http = http_client::get();
			auto request = http->create_request("GET");
		
			request->add_header("User-Agent", PFC_string_formatter() << "foo_musicbrainz/" << component_version);
			{
				std::lock_guard<std::mutex> guard(querying_mb);	// lock_guard is destroyed when exiting block

				uint64_t now = fileTimeToMilliseconds(pfc::fileTimeNow());
				if (now < last_http_request + 1000 && !abort.is_aborting()) {
					uint32_t delay = (uint32_t)(last_http_request + 1000 - now);
					Sleep(delay);
				}
				last_http_request = fileTimeToMilliseconds(pfc::fileTimeNow());
			}
			if (!abort.is_aborting()) {
				auto response = request->run_ex(url, abort);
				response->read_string_raw(buffer, abort);

				// Get string
				json j = json::parse(buffer.get_ptr(), nullptr, false);
				if (j.is_object())
				{
					cache.set(url, buffer);
					return j;
				}

				http_reply::ptr ptr;
				if (response->cast(ptr))
				{
					ptr->get_status(buffer);
				}

			}

			return json();
		}
		catch (exception_aborted) {
			// ignore
			FB2K_console_formatter() << component_title << ": User aborted";
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

	str8 query::get_url() {
		return url;
	}
}
