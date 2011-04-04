#include "foo_musicbrainz.h"

RequestURL::RequestURL()
{
	url = "ws/1/release/?type=xml";
}

RequestURL::RequestURL(const char *mbid)
{
	url = "ws/1/release/";
	url += mbid;
	url += "?type=xml";
}

void RequestURL::AddParam(const char *param, const char *value, bool encode)
{
	url += "&";
	url += param;
	url += "=";
	url += encode ? URLEncode(value) : value;
}

void RequestURL::AddParam(const char *param, int value, bool encode)
{
	char charvalue[15];
	sprintf(charvalue, "%d", value);
	AddParam(param, charvalue, false);
}

const char *RequestURL::GetURL()
{
	return url;
}

inline char RequestURL::ToHex(char c)
{
	return c < 0xa ? '0' + c : 'a' - 0xa + c;
}

pfc::string8 RequestURL::URLEncode(const char *in)
{
	pfc::string8 out;
	out.prealloc(strlen(in) * 3 + 1);

	for (register const char *tmp = in; *tmp != '\0'; tmp++)
	{
		if (isalnum(*tmp))
		{
			out.add_char(*tmp);
		}
		else if (isspace(*tmp))
		{
			out.add_char('+');
		}
		else
		{
			out.add_char('%');
			out.add_char(ToHex(((unsigned char)*tmp)>>4));
			out.add_char(ToHex(((unsigned char)*tmp)%16));
		}
	}
	out.add_char('\0');
	return out;
}
