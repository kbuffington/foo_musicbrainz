#pragma once

class RequestURL
{
public:
	RequestURL();
	RequestURL(const char *mbid);
	void AddParam(const char *param, const char *value, bool encode = true);
	void AddParam(const char *param, int value, bool encode = true);
	const char *GetURL();
	operator const wchar_t *();

private:
	pfc::string8 url;
	inline char ToHex(char c);
	pfc::string8 URLEncode(const char *in);

};
