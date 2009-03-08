#include "foo_musicbrainz.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_connection_error, pfc::exception, "Error connecting to musicbrainz.org.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_xml_parsing, pfc::exception, "Error parsing XML.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_no_releases, pfc::exception, "No releases.")

foo_mb_request_thread::foo_mb_request_thread(const char *_url, HWND window)
	: tagger_dialog(window)
{
	collection = ((mbCollection *)GetProp(tagger_dialog, L"Collection"));
	int url_len = MultiByteToWideChar(CP_UTF8, 0, _url, -1, NULL, 0);
	url = new wchar_t [url_len];
	MultiByteToWideChar(CP_UTF8, 0, _url, -1, url, url_len);
	hSession = hConnect = hRequest = NULL;
}

void foo_mb_request_thread::get_parse_xml(wchar_t *url, abort_callback & p_abort, int release_number) 
{
	pfc::string8 buffer;
	char *tmp;
	unsigned long size, downloaded;

	hSession = WinHttpOpen(L"foo_musicbrainz/" _T(COMPONENT_VERSION), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (hSession == NULL) throw exception_foo_mb_connection_error();
	p_abort.check();
	hConnect = WinHttpConnect(hSession, L"musicbrainz.org", INTERNET_DEFAULT_HTTP_PORT, 0);
	if (hConnect == NULL) throw exception_foo_mb_connection_error();
	p_abort.check();
	hRequest = WinHttpOpenRequest(hConnect, L"GET", url, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	if (hRequest == NULL) throw exception_foo_mb_connection_error();	
	p_abort.check();
	if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) throw exception_foo_mb_connection_error();
	p_abort.check();
	if (!WinHttpReceiveResponse(hRequest, NULL) || !WinHttpQueryDataAvailable(hRequest, &size)) throw exception_foo_mb_connection_error();
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
	if (hRequest != NULL) WinHttpCloseHandle(hRequest);
	if (hConnect != NULL) WinHttpCloseHandle(hConnect);
	if (hSession != NULL) WinHttpCloseHandle(hSession);

	ticpp::Document xml;
	mbRelease *mbr;
	ticpp::Element *releases;

	// Parsing XML
	try
	{
		xml.Parse((char *)buffer.get_ptr(), true, TIXML_ENCODING_UTF8);
	}
	// Not valid XML?
	catch (ticpp::Exception)
	{
		pfc::string8 error = "Error parsing XML, response from musicbrainz.org:\n\n";
		error += buffer;
		throw exception_foo_mb_xml_parsing(error);
	}

	releases = xml.FirstChildElement("metadata")->FirstChildElement("release-list", false) ? xml.FirstChildElement("metadata")->FirstChildElement("release-list") : xml.FirstChildElement("metadata");
	
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
		if (release_number >= 0)
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
					pfc::string8 str = release_event->GetAttribute("date").data();
					if (strcmp(str, "") != 0 && str[0] != '0' && 
						(strcmp(mbr->getDate(), "") == 0 ||
						 strcmp(str, mbr->getDate()) < 0 ||
						 (strlen(str) > strlen(mbr->getDate()) && memcmp(str, mbr->getDate(), strlen(mbr->getDate())) == 0)
						 )
						)
					{
						mbr->setDate(release_event->GetAttribute("date").data());
					}
				}
			} catch (ticpp::Exception) {}
		}

		// Adding tracks
		ticpp::Iterator<ticpp::Element> track;
		//try {
		for (track = track.begin(release->FirstChildElement("track-list")); track != track.end(); track++)
		{
			mbTrack *mbt = mbr->addTrack(track->FirstChildElement("title")->FirstChild()->Value().data(), track->GetAttribute("id").data());
			if (track->FirstChildElement("artist", false))
			{
				mbt->setArtist(track->FirstChildElement("artist")->FirstChildElement("name")->FirstChild()->Value().data());
				mbr->va = true;
			}
		}
		//} catch (ticpp::Exception e) {
		//	if (release_number >= 0)
		//	{
		//		//throw e;
		//	}
		//}
	}
}

void foo_mb_request_thread::run(threaded_process_status & p_status,abort_callback & p_abort)
{
	try {
		get_parse_xml(url, p_abort);
		p_status.set_progress(p_status.progress_max / (collection->getReleasesCount() + 1));
		for (unsigned int i = 0; i < collection->getReleasesCount(); i++)
		{
			mbRelease *release = collection->getRelease(i);
			if (release->getTracksCount() == 0)
			{
				uconvert str;
				pfc::string8 str2;
				str2 = "/ws/1/release/";
				str2 += release->getId();
				str2 += "?type=xml&inc=artist+tracks";
				get_parse_xml(str.ToUtf16(str2), p_abort, i);
				p_status.set_progress(p_status.progress_max / (collection->getReleasesCount() + 1) * (1 + i));
			}
		}
		PostMessage(tagger_dialog, WM_FOO_MB_UPDATE_RELEASES_LIST, 0, 0);
		ShowWindow(tagger_dialog, SW_SHOW);
	} catch (exception_foo_mb_connection_error e) {
		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
		popup_message::g_show(e.g_what(), COMPONENT_TITLE, popup_message::icon_error);
	} catch (exception_aborted) {
		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
	} catch (exception_foo_mb_xml_parsing e) {
		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
		pfc::string8 error_sting = e.what();
		popup_message::g_show(error_sting, COMPONENT_TITLE, popup_message::icon_error);
	} catch (exception_foo_mb_no_releases e) {
		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
		popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
	} catch (ticpp::Exception e) {
		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
		popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
	}
	if (hRequest != NULL) WinHttpCloseHandle(hRequest);
	if (hConnect != NULL) WinHttpCloseHandle(hConnect);
	if (hSession != NULL) WinHttpCloseHandle(hSession);
	delete [] url;
}
