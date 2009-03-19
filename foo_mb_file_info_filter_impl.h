class foo_mb_file_info_filter_impl : public file_info_filter {
public:
	foo_mb_file_info_filter_impl(HWND tagger_dialog, mbCollection *_mbc);
	bool apply_filter(metadb_handle_ptr p_location,t_filestats p_stats,file_info & info);
	~foo_mb_file_info_filter_impl();

private:
	HWND tagger_dialog;
	mbRelease *release;
	mbCollection *mbc;
	pfc::list_base_t<metadb_handle_ptr> *tracklist;
};
