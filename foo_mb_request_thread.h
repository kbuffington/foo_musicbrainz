class foo_mb_request_thread : public threaded_process_callback
{
public:
	enum {
		flag_tracks_only					= 1 << 0,
		flag_show_window					= 1 << 1
	};

	foo_mb_request_thread(const char *_url, HWND window, int _flags = flag_show_window, int _release_number = -1);
	void parse_xml(char *xml_string);
	void run(threaded_process_status & p_status,abort_callback & p_abort);

private:
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;
	wchar_t *url;
	HWND tagger_dialog;
	mbCollection *collection;
	int flags;
	int release_number;
};
