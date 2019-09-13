#pragma once

class mb_query
{
public:
	mb_query(const char* entity, const char* id = "");

	json lookup(abort_callback& p_abort);
	void add_param(const char* param, const char* value);

private:
	str8 url;
};
