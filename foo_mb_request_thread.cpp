#include <regex>
#include "foo_musicbrainz.h"

using namespace std::tr1;

extern cfg_bool cfg_short_date;
extern cfg_bool cfg_no_feat;

PFC_DECLARE_EXCEPTION(exception_foo_mb_connection_error, pfc::exception, "Error connecting to musicbrainz.org.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_xml_parsing, pfc::exception, "Error parsing XML.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_no_releases, pfc::exception, "No releases.")

foo_mb_request_thread::foo_mb_request_thread(const char *_url, HWND window, ReleaseList *_mbc)
	: tagger_dialog(window), mbc(_mbc)
{
	int url_len = MultiByteToWideChar(CP_UTF8, 0, _url, -1, NULL, 0);
	url = new wchar_t [url_len];
	MultiByteToWideChar(CP_UTF8, 0, _url, -1, url, url_len);
	hSession = hConnect = hRequest = NULL;
}

void foo_mb_request_thread::get_parse_xml(const wchar_t *url, abort_callback & p_abort, int release_number) 
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
	Release *mbr;
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
	for (release = release.begin(releases); release != release.end(); ++release)
	{
		if (release_number >= 0)
		{
			mbr = mbc->getRelease(release_number);
		}
		else
		{
			std::string full_title(release->FirstChildElement("title")->FirstChild()->Value());
			regex rx("^(.+?)(?:\\s+\\(disc (\\d+)(?::\\s(.+))?\\))?$");
			pfc::string8 title, discnumber, discsubtitle;
			cmatch res;
			regex_search(full_title.c_str(), res, rx);
			title.set_string(res[1].first, res[1].second - res[1].first);
			discnumber.set_string(res[2].first, res[2].second - res[2].first);
			discsubtitle.set_string(res[3].first, res[3].second - res[3].first);
			mbr = mbc->addRelease(
				title,
				release->GetAttribute("id").data(),
				release->FirstChildElement("artist")->FirstChildElement("name")->FirstChild()->Value().data(),
				release->FirstChildElement("artist")->GetAttribute("id").data()
				);
			mbr->setDisc(discnumber);
			mbr->setDiscSubtitle(discsubtitle);
			try {
				pfc::string8 type = release->GetAttribute("type").data();
				if (!type.is_empty())
				{
					pfc::string8 status = type;
					t_size space = type.find_first(" ");
					if (space != ~0)
					{
						status = type.get_ptr()+space+1;
						type.truncate(space);
					}
					mbr->setType(type);
					mbr->setStatus(status);
				}
			} catch (ticpp::Exception) {}
			try {
				ticpp::Iterator<ticpp::Element> release_event;
				for (release_event = release_event.begin(release->FirstChildElement("release-event-list")); release_event != release_event.end(); ++release_event)
				{
					pfc::string8 str = release_event->GetAttribute("date").data();
					if (strcmp(str, "") != 0 && str[0] != '0' && 
						(strcmp(mbr->getDate(), "") == 0 ||
						 strcmp(str, mbr->getDate()) < 0 ||
						 (strlen(str) > strlen(mbr->getDate()) && memcmp(str, mbr->getDate(), strlen(mbr->getDate())) == 0)
						 )
						)
					{
						if (cfg_short_date && str.length() > 4) str.truncate(4);
						mbr->setDate(str);
					}
				}
			} catch (ticpp::Exception) {}
		}

		// Adding tracks
		ticpp::Iterator<ticpp::Element> track;
		//try {
		for (track = track.begin(release->FirstChildElement("track-list")); track != track.end(); ++track)
		{
			pfc::string8 track_title(track->FirstChildElement("title")->FirstChild()->Value().data());
			if (cfg_no_feat)
			{
				regex rx("^(.+?)(\\s+\\(feat\\.\\s+.+\\))?$");
				cmatch res;
				regex_search(track_title.get_ptr(), res, rx);
				pfc::string8 tmp;
				tmp.set_string(res[1].first, res[1].second - res[1].first);
				track_title = tmp;
			}
			Track *mbt = mbr->addTrack(track_title.get_ptr(), track->GetAttribute("id").data());

			if (track->FirstChildElement("artist", false))
			{
				mbt->setArtist(track->FirstChildElement("artist")->FirstChildElement("name")->FirstChild()->Value().data());
				mbt->setArtistId(track->FirstChildElement("artist")->GetAttribute("id").data());
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
		p_status.set_progress(p_status.progress_max / (mbc->getReleasesCount() + 1));
		for (unsigned int i = 0; i < mbc->getReleasesCount(); i++)
		{
			Release *release = mbc->getRelease(i);
			if (release->getTracksCount() == 0)
			{
				RequestURL request_url(release->getId());
				request_url.AddParam("inc", "artist+tracks", false);
				get_parse_xml(request_url, p_abort, i);
				p_status.set_progress(p_status.progress_max / (mbc->getReleasesCount() + 1) * (1 + i));
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
