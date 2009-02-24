class foo_mb_menu : public contextmenu_item_simple
{
public:
	unsigned get_num_items();
	void get_item_name(unsigned p_index,pfc::string_base & p_out);
	void get_item_default_path(unsigned p_index,pfc::string_base & p_out);
	void context_command(unsigned p_index,metadb_handle_list_cref p_data,const GUID& p_caller);
	GUID get_item_guid(unsigned p_index);
	bool get_item_description(unsigned p_index,pfc::string_base & p_out);
	bool context_get_display(unsigned p_index,metadb_handle_list_cref p_data,pfc::string_base & p_out,unsigned & p_displayflags,const GUID & p_caller);
};
