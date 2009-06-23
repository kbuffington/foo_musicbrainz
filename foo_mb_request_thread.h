class foo_mb_request_thread : public threaded_process_callback
{
public:
	foo_mb_request_thread(const char *_url, HWND window, ReleaseList *_mbc);
	void get_parse_xml(const wchar_t *url, abort_callback & p_abort, int release_number = -1);
	void run(threaded_process_status & p_status,abort_callback & p_abort);

private:
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;
	wchar_t *url;
	HWND tagger_dialog;
	ReleaseList *mbc;
};
