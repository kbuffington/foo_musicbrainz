class foo_mb_file_info_filter_impl : public file_info_filter {
public:
	foo_mb_file_info_filter_impl(HWND tagger_dialog);
	bool apply_filter(metadb_handle_ptr p_location,t_filestats p_stats,file_info & info);
	~foo_mb_file_info_filter_impl();

private:
	HWND tagger_dialog;
	mbRelease *release;
	mbCollection *collection;
	metadb_handle_list *tracklist;
	//pfc::array_t<file_info_impl> m_infos;
};

