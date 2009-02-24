class uconvert
{
public:
	uconvert();
	wchar_t *ToUtf16(const char *ptr);
	char *ToUtf8(const wchar_t *ptr);
	~uconvert();

private:
	void *ptr;
	size_t size;
};
