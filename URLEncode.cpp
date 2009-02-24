#include "foo_musicbrainz.h"

inline char toHex(char c)
{
	return c < 0xa ? ('0' + c) : ('a' - 0xa + c);
}

pfc::string8 URLEncode(const char *in)
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
			out.add_char(toHex(((unsigned char)*tmp)>>4));
			out.add_char(toHex(((unsigned char)*tmp)%16));
		}
	}
	out.add_char('\0');
	return out;
}
