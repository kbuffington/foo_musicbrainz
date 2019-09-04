#pragma once

class mb_query {
public:
	mb_query(const char* entity, const char* id = "");

	json lookup(abort_callback& p_abort);
	void add_param(const char* param, const char* value, bool encode = true);

private:
	char to_hex(char);
	str8 url_encode(const char* in);

	str8 url;
};
