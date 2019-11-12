#pragma once

namespace mb
{
	class query
	{
	public:
		query(const char* entity, const char* id = "");

		json lookup(abort_callback& p_abort);
		void add_param(const char* param, const char* value);

	private:
		str8 url;
	};
}
