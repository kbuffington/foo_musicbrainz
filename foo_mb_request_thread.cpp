#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_connection_error, pfc::exception, "Error connecting to musicbrainz.org.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_xml_parsing, pfc::exception, "Error parsing XML, response from musicbrainz.org:\n\n")
PFC_DECLARE_EXCEPTION(exception_foo_mb_no_releases, pfc::exception, "No releases.")

foo_mb_request_thread::foo_mb_request_thread(const char *_url, HWND window, int _flags, int _release_number)
	: tagger_dialog(window), flags(_flags), release_number(_release_number)
{
	collection = ((mbCollection *)GetProp(tagger_dialog, L"Collection"));
	int url_len = MultiByteToWideChar(CP_UTF8, 0, _url, -1, NULL, 0);
	url = new wchar_t [url_len];
	MultiByteToWideChar(CP_UTF8, 0, _url, -1, url, url_len);
	hSession = hConnect = hRequest = NULL;
}

void foo_mb_request_thread::parse_xml(char *xml_string) 
{
	ticpp::Document xml;
	mbRelease *mbr;
	ticpp::Element *releases;

	// Parsing XML
	try
	{
		xml.Parse(xml_string, true, TIXML_ENCODING_UTF8);
	}
	// Not valid XML?
	catch (ticpp::Exception)
	{
		throw exception_foo_mb_xml_parsing();
	}

	try
	{
		releases = xml.FirstChildElement("metadata")->FirstChildElement("release-list");
	}
	catch (ticpp::Exception)
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
		if (flags & flag_tracks_only)
		{
			mbr = collection->getRelease(release_number);
		}
		else
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

		// Adding tracks
		ticpp::Iterator<ticpp::Element> track;
		for (track = track.begin(release->FirstChildElement("track-list")); track != track.end(); track++)
		{
			mbr->addTrack(track->FirstChildElement("title")->FirstChild()->Value().data(), track->GetAttribute("id").data());
		}
	}
}

void foo_mb_request_thread::run(threaded_process_status & p_status,abort_callback & p_abort)
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
		if (flags & flag_tracks_only)
		{
			PostMessage(tagger_dialog, WM_FOO_MB_UPDATE_RELEASE, 0, 0);
		}
		else
		{
			PostMessage(tagger_dialog, WM_FOO_MB_UPDATE_RELEASES_LIST, 0, 0);
		}
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
