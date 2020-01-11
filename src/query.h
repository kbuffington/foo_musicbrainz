#pragma once

namespace mb
{
	class query
	{
	public:
		query(pfc::stringp entity, pfc::stringp id = "");

		json lookup(abort_callback& abort);
		void add_param(pfc::stringp param, pfc::stringp value);

	private:
		str8 url;
	};
}
