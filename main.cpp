#include "foo_musicbrainz.h"

DECLARE_COMPONENT_VERSION(COMPONENT_TITLE, COMPONENT_VERSION, "Copyright (C) 2009 Dremora");

PFC_DECLARE_EXCEPTION(exception_foo_mb_connection_error, pfc::exception, "Error connecting to musicbrainz.org.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_xml_parsing, pfc::exception, "Error parsing XML, response from musicbrainz.org:\n\n")
PFC_DECLARE_EXCEPTION(exception_foo_mb_no_releases, pfc::exception, "No releases.")

unsigned long WM_FOO_MB;

class uconvert
{
public:
	uconvert()
	{
		size = 0;
	}

	wchar_t *ToUtf16(const char *ptr)
	{
		if (size > 0) delete [] this->ptr;
		size = MultiByteToWideChar(CP_UTF8, 0, ptr, -1, (LPWSTR)this->ptr, 0);
		this->ptr = new wchar_t [size];
		MultiByteToWideChar(CP_UTF8, 0, ptr, -1, (LPWSTR)this->ptr, size);
		return (wchar_t *)this->ptr;
	}

	char *ToUtf8(const wchar_t *ptr)
	{
		if (size > 0) delete [] this->ptr;
		size = WideCharToMultiByte(CP_UTF8, 0, ptr, -1, (LPSTR)this->ptr, 0, NULL, NULL);
		this->ptr = new char [size];
		WideCharToMultiByte(CP_UTF8, 0, ptr, -1, (LPSTR)this->ptr, size, NULL, NULL);
		return (char *)this->ptr;
	}

	~uconvert()
	{
		if (size > 0) delete [] this->ptr;
	}

private:
	void *ptr;
	size_t size;
};

HWND list_view_edit::TableEdit_GetParentWnd() const
{
	return window;
}

void list_view_edit::start(t_size item, t_size subItem)
{
	TableEdit_Start(item, subItem);
}

list_view_edit::list_view_edit(HWND _window)
{
	window = _window;
}

bool list_view_edit::TableEdit_IsColumnEditable(t_size subItem) const
{
	if (subItem == 0 && TableEdit_GetColumnCount() == 2) return false;
	else return true;
}

void list_view_edit::TableEdit_Finished()
{
}

class foo_mb_request_thread : public threaded_process_callback
{
public:
	foo_mb_request_thread(const char *_url, HWND window, int _flags, int _release_number = -1)
		: tagger_dialog(window), flags(_flags), release_number(_release_number)
	{
		collection = ((mbCollection *)GetProp(tagger_dialog, L"Collection"));
		int url_len = MultiByteToWideChar(CP_UTF8, 0, _url, -1, NULL, 0);
		url = new wchar_t [url_len];
		MultiByteToWideChar(CP_UTF8, 0, _url, -1, url, url_len);
		hSession = hConnect = hRequest = NULL;
	}

	enum {
		flag_single_release					= 1 << 0,
		flag_show_window					= 1 << 1
	};

	void parse_xml(char *xml_string) 
	{
		ticpp::Document xml;
		mbRelease *mbr;
		mbTrack *mbt;

		try
		{
			xml.Parse(xml_string, true, TIXML_ENCODING_UTF8);
		}
		catch (ticpp::Exception)
		{
			throw exception_foo_mb_xml_parsing();
		}
		ticpp::Element *releases;
		try
		{
			releases = xml.FirstChildElement("metadata")->FirstChildElement("release-list");
		}
		catch (ticpp::Exception) {
		{
			releases = xml.FirstChildElement("metadata");
		}
		
		try
		{
			releases->FirstChildElement("release");
		}
		catch (ticpp::Exception)
		{
			throw exception_foo_mb_no_releases();
		}

		// Adding releases
		ticpp::Iterator<ticpp::Element> release;
		for (release = release.begin(releases); release != release.end(); release++)
		{
			if (release_number == -1)
			{
				mbr = collection->addRelease(
					release->FirstChildElement("title")->FirstChild()->Value().data(),
					release->GetAttribute("id").data(),
					release->FirstChildElement("artist")->FirstChildElement("name")->FirstChild()->Value().data(),
					release->FirstChildElement("artist")->GetAttribute("id").data()
					);
				try {
					ticpp::Iterator<ticpp::Element> release_event;
					for (release_event = release_event.begin(release->FirstChildElement("release-event-list")); release_event != release_event.end(); release_event++)
					{
						if (release_event->GetAttribute("date").data()[0] != '\0')
						{
							mbr->setDate(release_event->GetAttribute("date").data());
							break;
						}
					}
				} catch (ticpp::Exception) {}
			}
			else
			{
				mbr = collection->getRelease(release_number);
			}

			// Adding tracks
			ticpp::Iterator<ticpp::Element> track;
			//int track_number = 1;
			for (track = track.begin(release->FirstChildElement("track-list")); track != track.end(); track++)
			{
				mbr->addTrack(
					track->FirstChildElement("title")->FirstChild()->Value().data(),
					track->GetAttribute("id").data()
					);
			}
		}
	}

	void run(threaded_process_status & p_status,abort_callback & p_abort)
	{
		unsigned long size, downloaded;
		pfc::string8 buffer;
		char *tmp;
		try {
			hSession = WinHttpOpen(L"foo_musicbrainz/" _T(COMPONENT_VERSION), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (hSession == NULL) throw exception_foo_mb_connection_error();
			p_status.set_progress(p_status.progress_max*1/6);
			p_abort.check();
			hConnect = WinHttpConnect(hSession, L"musicbrainz.org", INTERNET_DEFAULT_HTTP_PORT, 0);
			if (hConnect == NULL) throw exception_foo_mb_connection_error();
			p_status.set_progress(p_status.progress_max*2/6);
			p_abort.check();
			hRequest = WinHttpOpenRequest(hConnect, L"GET", url, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
			if (hRequest == NULL) throw exception_foo_mb_connection_error();	
			p_status.set_progress(p_status.progress_max*3/6);
			p_abort.check();
			if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) throw exception_foo_mb_connection_error();
			p_status.set_progress(p_status.progress_max*4/6);
			p_abort.check();
			if (!WinHttpReceiveResponse(hRequest, NULL) || !WinHttpQueryDataAvailable(hRequest, &size)) throw exception_foo_mb_connection_error();
			p_status.set_progress(p_status.progress_max*5/6);
			p_abort.check();
			while (size > 0)
			{
				tmp = new char[size+1];
				WinHttpReadData(hRequest, tmp, size, &downloaded);
				tmp[downloaded] = '\0';
				buffer += tmp;
				delete [] tmp;
				WinHttpQueryDataAvailable(hRequest, &size);
				p_abort.check();
			};
			p_status.set_progress(p_status.progress_max);
			parse_xml((char *)buffer.get_ptr());
			PostMessage(tagger_dialog, WM_FOO_MB, 0, 0);
			if (flags & flag_show_window) ShowWindow(tagger_dialog, SW_SHOW);
		} catch (exception_foo_mb_connection_error e) {
			if (flags & flag_show_window) PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
			popup_message::g_show(e.g_what(), COMPONENT_TITLE, popup_message::icon_error);
		} catch (exception_aborted) {
			if (flags & flag_show_window) PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
		} catch (exception_foo_mb_xml_parsing e) {
			if (flags & flag_show_window) PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
			pfc::string8 error_sting = e.what();
			error_sting += buffer.get_ptr();
			popup_message::g_show(error_sting, COMPONENT_TITLE, popup_message::icon_error);
		} catch (exception_foo_mb_no_releases e) {
			if (flags & flag_show_window) PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
			popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
		} catch (ticpp::Exception e) {
			if (flags & flag_show_window) PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
			popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
		}
		if (hRequest != NULL) WinHttpCloseHandle(hRequest);
		if (hConnect != NULL) WinHttpCloseHandle(hConnect);
		if (hSession != NULL) WinHttpCloseHandle(hSession);
		delete [] url;
	}

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

class foo_mb_menu : public contextmenu_item_simple
{
public:
	unsigned get_num_items()
	{
		return 2;
	}

	void get_item_name(unsigned p_index,pfc::string_base & p_out)
	{
		static const char *item_name[] = { "Get Tags From MusicBrainz (by TOC)", "Get Tags From MusicBrainz (by data)", "Get Tags From MusicBrainz (custom query)" };
		p_out = item_name[p_index];
	}

	void get_item_default_path(unsigned p_index,pfc::string_base & p_out)
	{
		p_out = "Tagging";
	}

	void context_command(unsigned p_index,metadb_handle_list_cref p_data,const GUID& p_caller)
	{
		unsigned int count = p_data.get_count();
		if (count > 99)
		{
			popup_message::g_show("Please select no more than 99 tracks.", COMPONENT_TITLE, popup_message::icon_error);
			return;
		}
		switch (p_index)
		{
		case 0:
			{
				unsigned int *tracks_lengths = new unsigned int [count];
				const file_info *info;
				__int64 samples;
				for (t_size i = 0; i < count; i++)
				{
					p_data.get_item(i)->metadb_lock();
					if (!p_data.get_item(i)->get_info_locked(info))
					{
						p_data.get_item(i)->metadb_unlock();
						delete [] tracks_lengths;
						return;
					}
					samples = info->info_get_length_samples();
					p_data.get_item(i)->metadb_unlock();
					if (samples % 588 != 0)
					{
						popup_message::g_show("Track length in samples must be divisible by 588.", COMPONENT_TITLE, popup_message::icon_error);
						delete [] tracks_lengths;
						return;
					} 
					tracks_lengths[i] = (unsigned int)samples / 588;
				}
				char *discid = get_discid(count, tracks_lengths, 150);
				delete [] tracks_lengths;
				
				pfc::string8 url = "ws/1/release/?type=xml&discid=";
				url += discid;
				delete [] discid;
				HWND tagger_dialog = CreateDialog(core_api::get_my_instance(), MAKEINTRESOURCE(IDD_TAGGER_DIALOG), core_api::get_main_window(), TaggerDialogProc);
				SetProp(tagger_dialog, L"Collection", new mbCollection(tagger_dialog, p_data));
				console::print(url);
				threaded_process::g_run_modeless(new service_impl_t<foo_mb_request_thread>(url, tagger_dialog), threaded_process::flag_show_progress | threaded_process::flag_show_abort, core_api::get_main_window(), "Quering information from MusicBrainz");
				break;
			}
		case 1:
			{
				const file_info *info;
				pfc::string8 artist, album, url;
				for (t_size i = 0; i < count; i++)
				{
					p_data.get_item(i)->metadb_lock();
					if (!p_data.get_item(i)->get_info_locked(info))
					{
						p_data.get_item(i)->metadb_unlock();
						return;
					}
					artist = info->meta_get("ARTIST", 0);
					album = info->meta_get("ALBUM", 0);
					if (artist.is_empty() || album.is_empty()) continue;
					else break;
					p_data.get_item(i)->metadb_unlock();
				}
				if (artist.is_empty() || album.is_empty())
				{
					popup_message::g_show("Selected tracks do not have any Artist and Album tags.", COMPONENT_TITLE, popup_message::icon_error);
					return;
				}
				url = "ws/1/release/?type=xml&artist=";
				url += URLEncode(artist);
				url += "&title=";
				url += URLEncode(album);

				HWND tagger_dialog = CreateDialog(core_api::get_my_instance(), MAKEINTRESOURCE(IDD_TAGGER_DIALOG), core_api::get_main_window(), TaggerDialogProc);
				SetProp(tagger_dialog, L"Collection", new mbCollection(tagger_dialog, p_data));
				console::print(url);
				threaded_process::g_run_modeless(new service_impl_t<foo_mb_request_thread>(url, tagger_dialog, false), threaded_process::flag_show_progress | threaded_process::flag_show_abort, core_api::get_main_window(), "Quering information from MusicBrainz");
				break;
			}
		case 2:
			popup_message::g_show("Sorry, not yet implemented.", COMPONENT_TITLE, popup_message::icon_error);
			break;
		}

	}

	GUID get_item_guid(unsigned p_index)
	{
		static const GUID guid_foo_mb_menu[] = { { 0x3ca8395b, 0x694e, 0x4845, { 0xb5, 0xea, 0x56, 0x30, 0x5e, 0x7c, 0x24, 0x48 } }, { 0x77f1f5cd, 0xf295, 0x4ef4, { 0xba, 0x7b, 0xc7, 0x70, 0xaa, 0xc6, 0xd0, 0x1e } }, { 0x9c4dac6a, 0xe571, 0x4cd4, { 0x82, 0xd8, 0x51, 0x31, 0x44, 0x94, 0xde, 0xc5 } } };
		return guid_foo_mb_menu[p_index];
	}

	bool get_item_description(unsigned p_index,pfc::string_base & p_out)
	{
		static const char *item_description[] = { "Queries MusicBrainz server for tags for a complete CD using TOC.", "Queries MusicBrainz server for tags for a complete CD using tracks data.", "Queries MusicBrainz server for tags for a complete CD using custom search query." };
		p_out = item_description[p_index];
		return true;
	}

	bool context_get_display(unsigned p_index,metadb_handle_list_cref p_data,pfc::string_base & p_out,unsigned & p_displayflags,const GUID & p_caller)
	{
		PFC_ASSERT(p_index>=0 && p_index<get_num_items());
		unsigned int count = p_data.get_count();
		if (count <= 99)
		{
			if (p_index != 0)
			{
				get_item_name(p_index,p_out);
				return true;
			}
			const file_info *info;
			for (t_size i = 0; i < count; i++)
			{
				p_data.get_item(i)->metadb_lock();
				if (!p_data.get_item(i)->get_info_locked(info) || info->info_get_length_samples() % 588 != 0)
				{
					p_data.get_item(i)->metadb_unlock();
					return false;
				}
				p_data.get_item(i)->metadb_unlock();
			}
			get_item_name(p_index,p_out);
			return true;
		} else {
			return false;
		}
	}
};

static contextmenu_item_factory_t<foo_mb_menu> g_foo_mb_menu_factory;

class foo_mb_initquit : public initquit
{
	virtual void on_init()
	{
		WM_FOO_MB = RegisterWindowMessage(L"foo_mb");
	}

	virtual void on_quit()
	{
	}
};

static initquit_factory_t<foo_mb_initquit> g_foo_mb_initquit_factory;

BOOL CALLBACK TaggerDialogProc(HWND tagger_dialog, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//console::printf("0x%04x: %u", Message, GetCurrentThreadId());

	if (Message == WM_FOO_MB)
	{
		HWND release_list = GetDlgItem(tagger_dialog, IDC_RELEASE_LIST);
		HWND track_list = GetDlgItem(tagger_dialog, IDC_TRACK_LIST);
		LVITEM list_item;
		uconvert str;
		mbCollection *mbc = (mbCollection *)GetProp(tagger_dialog, L"Collection");
		wchar_t track_number_str[10];

		list_item.mask = LVIF_TEXT;
		list_item.iSubItem = 0;

		for (list_item.iItem = 0; list_item.iItem < (int)mbc->getReleasesCount(); list_item.iItem++)
		{
			list_item.pszText = str.ToUtf16(mbc->getRelease(list_item.iItem)->getArtist());
			ListView_InsertItem(release_list, &list_item);
			ListView_SetItemText(release_list, list_item.iItem, 1, str.ToUtf16(mbc->getRelease(list_item.iItem)->getTitle()));
			ListView_SetItemText(release_list, list_item.iItem, 2, str.ToUtf16(mbc->getRelease(list_item.iItem)->getDate()));
		}

		mbRelease *release = mbc->getRelease(0);
		for (list_item.iItem = 0; list_item.iItem < (int)release->getTracksCount(); list_item.iItem++)
		{
			//column 1 - track_number
			wsprintf(track_number_str, L"%u", list_item.iItem+1);
			list_item.pszText = track_number_str;
			ListView_InsertItem(track_list, &list_item);

			//column 2 - title
			ListView_SetItemText(track_list, list_item.iItem, 1, str.ToUtf16(release->getTrack(list_item.iItem)->getTitle()));
		}

		if (release->getTracksCount() == 0)
		{
			pfc::string8 url = "ws/1/release/";
			url += URLEncode(mbc->getRelease(0)->getId());
			url += "?type=xml&inc=artist+release-events+tracks";
			threaded_process::g_run_modeless(new service_impl_t<foo_mb_request_thread>(url.get_ptr(), tagger_dialog, true, true, 0), threaded_process::flag_show_progress | threaded_process::flag_show_abort, core_api::get_main_window(), "Quering information from MusicBrainz");
		}

		pfc::string8 url;
		url = "<a href=\"http://musicbrainz.org/release/";
		url += release->getId();
		url += ".html\">MusicBrainz release page</a>";
		uSetDlgItemText(tagger_dialog, IDC_URL, url);
		return true;
	}

	switch(Message)
	{
	case WM_INITDIALOG:
		{

			HWND release_list = GetDlgItem(tagger_dialog, IDC_RELEASE_LIST);
			HWND track_list = GetDlgItem(tagger_dialog, IDC_TRACK_LIST);
			
			LVCOLUMN column_item;
			
			// List view styles
			ListView_SetExtendedListViewStyleEx(release_list, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
			ListView_SetExtendedListViewStyleEx(track_list, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);

			// Adding release list columns
			column_item.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
			wchar_t *release_list_columns[] = { L"Artist", L"Release", L"Date" };
			int col_size[] = { 180, 180, 70 };
			for (int i = 0; i < 3; i++) 
			{ 
				column_item.iSubItem = i;
				column_item.pszText = release_list_columns[i];
				column_item.cx = col_size[i];
				ListView_InsertColumn(release_list, i, &column_item);
			}

			// Adding track list columns
			// Fake column
			column_item.mask = 0;
			ListView_InsertColumn(track_list, 0, &column_item);

			column_item.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
			column_item.fmt = LVCFMT_RIGHT;
			column_item.cx = 30;
			column_item.pszText = L"#";
			ListView_InsertColumn(track_list, 1, &column_item);

			column_item.mask = LVCF_WIDTH | LVCF_TEXT;
			column_item.cx = 390;
			column_item.pszText = L"Title";
			ListView_InsertColumn(track_list, 2, &column_item);

			ListView_DeleteColumn(track_list, 0);

			//column_item.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 

			//column_item.iSubItem = 0;
			//column_item.pszText = L"#";
			//column_item.cx = 100;
			//if (i == 0) column_item.fmt = LVCFMT_RIGHT;
			//else column_item.fmt = LVCFMT_LEFT;
			//ListView_InsertColumn(track_list, i, &column_item);

			//wchar_t *track_list_columns[] = { L"#", L"Title" };
			//for (int i = 0; i < 2; i++) 
			//{ 
			//	column_item.iSubItem = i;
			//	column_item.pszText = track_list_columns[i];
			//	column_item.cx = 100;
			//	if (i == 0) column_item.fmt = LVCFMT_RIGHT;
			//	else column_item.fmt = LVCFMT_LEFT;
			//	ListView_InsertColumn(track_list, i, &column_item);
			//}

			break;
		}

	case WM_CLOSE:
		DestroyWindow(tagger_dialog);
		break;

	case WM_DESTROY:
		delete GetProp(tagger_dialog, L"Collection");
		RemoveProp(tagger_dialog, L"Collection");
		break;
		
	case WM_NOTIFY:// 
	/*	if (((LPNMHDR)lParam)->code <= LVN_FIRST && ((LPNMHDR)lParam)->code >= LVN_LAST && ((LPNMHDR)lParam)->code != LVN_ITEMCHANGING)
		{
			console::printf("0x%08x", ((LPNMHDR)lParam)->code);
		}*/
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_DBLCLK:
			if (((LPNMITEMACTIVATE)lParam)->iItem != -1 && ((LPNMHDR)lParam)->hwndFrom == GetDlgItem(tagger_dialog, IDC_RELEASE_LIST))
			{
				((mbCollection *)GetProp(tagger_dialog, L"Collection"))->getReleasesTable()->start(((LPNMITEMACTIVATE)lParam)->iItem, ((LPNMITEMACTIVATE)lParam)->iSubItem);
			}
			break;

		case LVN_ITEMCHANGED:
			if (((LPNMLISTVIEW)lParam)->hdr.hwndFrom == GetDlgItem(tagger_dialog, IDC_RELEASE_LIST) && ((LPNMLISTVIEW)lParam)->iItem != -1 && ((LPNMLISTVIEW)lParam)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)lParam)->uNewState & LVIS_SELECTED)
			{
				if (((mbCollection *)GetProp(tagger_dialog, L"Collection"))->getCurrentRelease() != ((LPNMITEMACTIVATE)lParam)->iItem)
				{
					
					((mbCollection *)GetProp(tagger_dialog, L"Collection"))->setCurrentRelease(((LPNMITEMACTIVATE)lParam)->iItem);
					// TODO
				}
				
			}
			break;

		case LVN_ITEMCHANGING:
			console::printf("Selected item %u.%u. Changed: %u", ((LPNMITEMACTIVATE)lParam)->iItem, ((LPNMITEMACTIVATE)lParam)->iSubItem, ((LPNMITEMACTIVATE)lParam)->uChanged);
			break;

		case NM_CLICK:
			if (((LPNMITEMACTIVATE)lParam)->iItem != -1 && ((LPNMHDR)lParam)->hwndFrom == GetDlgItem(tagger_dialog, IDC_TRACK_LIST) && ((LPNMITEMACTIVATE)lParam)->iSubItem != 0)
			{
				((mbCollection *)GetProp(tagger_dialog, L"Collection"))->getTracksTable()->start(((LPNMITEMACTIVATE)lParam)->iItem, ((LPNMITEMACTIVATE)lParam)->iSubItem);
				break;
			}

		case NM_RETURN:
			{
				if ((((LPNMHDR)lParam)->hwndFrom == GetDlgItem(tagger_dialog, IDC_URL)) && (((PNMLINK)lParam)->item.iLink == 0))
				{	
					ShellExecute(NULL, L"open", ((PNMLINK)lParam)->item.szUrl, NULL, NULL, SW_SHOW);
				}
				break;
			}

		default:
			return FALSE;
		}

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			DestroyWindow(tagger_dialog);
			break;

		case IDOK:
			{
				mbRelease *release = ((mbCollection *)GetProp(tagger_dialog, L"Collection"))->getRelease(((mbCollection *)GetProp(tagger_dialog, L"Collection"))->getCurrentRelease());

				pfc::list_t<const file_info*> info_list;
				char track_number_str[10];
				file_info_impl *info = new file_info_impl[release->getTracksCount()];

				metadb_handle_list *data = ((mbCollection *)GetProp(tagger_dialog, L"Collection"))->getData();
				
				for (unsigned int i = 0; i < release->getTracksCount(); i++)
				{
					data->get_item(i)->get_info(info[i]);
					info[i].meta_set("ARTIST", release->getArtist());
					info[i].meta_set("ALBUM", release->getTitle());
					info[i].meta_set("DATE", release->getDate());
					info[i].meta_set("TITLE", release->getTrack(i)->getTitle());
					sprintf(track_number_str, "%u", i+1);
					info[i].meta_set("TRACKNUMBER", track_number_str);
					sprintf(track_number_str, "%u", release->getTracksCount());
					info[i].meta_set("TOTALTRACKS", track_number_str);
					info_list.add_item(&info[i]);
				}
				static_api_ptr_t<metadb_io_v2>()->update_info_async_simple(*data, info_list, core_api::get_main_window(), metadb_io_v2::op_flag_delay_ui, NULL);
				DestroyWindow(tagger_dialog);
				break;
			}

		default:
			return FALSE;
		}

	default:
		return FALSE;
	}
	return TRUE;
}
