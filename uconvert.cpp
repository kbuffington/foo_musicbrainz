#include "foo_musicbrainz.h"

uconvert::uconvert()
{
	size = 0;
}

wchar_t *uconvert::ToUtf16(const char *ptr)
{
	if (size > 0) delete [] this->ptr;
	size = MultiByteToWideChar(CP_UTF8, 0, ptr, -1, (LPWSTR)this->ptr, 0);
	this->ptr = new wchar_t [size];
	MultiByteToWideChar(CP_UTF8, 0, ptr, -1, (LPWSTR)this->ptr, size);
	return (wchar_t *)this->ptr;
}

char *uconvert::ToUtf8(const wchar_t *ptr)
{
	if (size > 0) delete [] this->ptr;
	size = WideCharToMultiByte(CP_UTF8, 0, ptr, -1, (LPSTR)this->ptr, 0, NULL, NULL);
	this->ptr = new char [size];
	WideCharToMultiByte(CP_UTF8, 0, ptr, -1, (LPSTR)this->ptr, size, NULL, NULL);
	return (char *)this->ptr;
}

uconvert::~uconvert()
{
	if (size > 0) delete [] this->ptr;
}
