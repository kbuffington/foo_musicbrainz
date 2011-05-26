#include "foo_musicbrainz.h"
#include "Query.h"
#include "Parser.h"

using namespace foo_musicbrainz;

Query::Query(pfc::string8 id) {
	url = "/ws/2/release/";
	url << id;
}

#define TO_HEX(c) (c < 0xa ? '0' + c : 'a' - 0xa + c)

pfc::string8 Query::url_encode(pfc::string8 &in) {
	pfc::string8 out;
	out.prealloc(strlen(in) * 3 + 1);

	for (register const char *tmp = in; *tmp != '\0'; tmp++) {
		if (isalnum(*tmp)) {
			out.add_char(*tmp);
		} else if (isspace(*tmp)) {
			out.add_char('+');
		} else {
			out.add_char('%');
			out.add_char(TO_HEX(((unsigned char)*tmp)>>4));
			out.add_char(TO_HEX(((unsigned char)*tmp)%16));
		}
	}
	// out.add_char('\0'); really needed?
	return out;
}

void Query::get(pfc::string8 &buffer) {
	pfc::stringcvt::string_os_from_utf8 url_wide(url);
	HINTERNET hSession, hConnect, hRequest;

	try {
		hSession = WinHttpOpen(L"foo_musicbrainz/" _T(COMPONENT_VERSION), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession == NULL) throw exception_foo_mb_connection_error();
		// p_abort.check();
		hConnect = WinHttpConnect(hSession, L"musicbrainz.org", INTERNET_DEFAULT_HTTP_PORT, 0);
		if (hConnect == NULL) throw exception_foo_mb_connection_error();
		// p_abort.check();
		hRequest = WinHttpOpenRequest(hConnect, L"GET", url_wide, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		if (hRequest == NULL) throw exception_foo_mb_connection_error();	
		// p_abort.check();
		if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) throw exception_foo_mb_connection_error();
		// p_abort.check();
		unsigned long size;
		if (!WinHttpReceiveResponse(hRequest, NULL) || !WinHttpQueryDataAvailable(hRequest, &size)) throw exception_foo_mb_connection_error();
		// p_abort.check();
		while (size > 0) {
			char *tmp = new char[size];
			unsigned long downloaded;
			WinHttpReadData(hRequest, tmp, size, &downloaded);
			buffer.add_string(tmp, downloaded);
			delete [] tmp;
			WinHttpQueryDataAvailable(hRequest, &size);
			// p_abort.check();
		};
	} catch (std::exception e) {
		if (hRequest != NULL) WinHttpCloseHandle(hRequest);
		if (hConnect != NULL) WinHttpCloseHandle(hConnect);
		if (hSession != NULL) WinHttpCloseHandle(hSession);
		throw e;
	}

	if (hRequest != NULL) WinHttpCloseHandle(hRequest);
	if (hConnect != NULL) WinHttpCloseHandle(hConnect);
	if (hSession != NULL) WinHttpCloseHandle(hSession);
}

void Query::parse(pfc::string8 &buffer, ticpp::Document &xml) {
	//Release *mbr;
	//ticpp::Element *releases;

	// Parsing XML
	try {
		xml.Parse(buffer.get_ptr(), true, TIXML_ENCODING_UTF8);
		// Not valid XML?
	} catch (ticpp::Exception) {
		pfc::string8 error = "Error parsing XML, response from musicbrainz.org:\n\n";
		error << buffer;
		throw exception_foo_mb_xml_parsing(error);
	}

	//releases = xml.FirstChildElement("metadata")->FirstChildElement("release-list", false) ? xml.FirstChildElement("metadata")->FirstChildElement("release-list") : xml.FirstChildElement("metadata");
	//
	//try
	//{
	//	releases->FirstChildElement("release");
	//}
	//catch (ticpp::Exception)
	//{
	//	throw exception_foo_mb_no_releases();
	//}

	//// Adding releases
	//ticpp::Iterator<ticpp::Element> release;
	//for (release = release.begin(releases); release != release.end(); ++release)
	//{
	//	if (release_number >= 0)
	//	{
	//		mbr = mbc->getRelease(release_number);
	//	}
	//	else
	//	{
	//		std::string full_title(release->FirstChildElement("title")->FirstChild()->Value());
	//		regex rx("^(.+?)(?:\\s+\\(disc (\\d+)(?::\\s(.+))?\\))?$");
	//		pfc::string8 title, discnumber, discsubtitle;
	//		cmatch res;
	//		regex_search(full_title.c_str(), res, rx);
	//		title.set_string(res[1].first, res[1].second - res[1].first);
	//		discnumber.set_string(res[2].first, res[2].second - res[2].first);
	//		discsubtitle.set_string(res[3].first, res[3].second - res[3].first);
	//		mbr = mbc->addRelease(
	//			title,
	//			release->GetAttribute("id").data(),
	//			release->FirstChildElement("artist")->FirstChildElement("name")->FirstChild()->Value().data(),
	//			release->FirstChildElement("artist")->GetAttribute("id").data()
	//			);
	//		mbr->setDisc(discnumber);
	//		mbr->setDiscSubtitle(discsubtitle);
	//		try {
	//			pfc::string8 type = release->GetAttribute("type").data();
	//			if (!type.is_empty())
	//			{
	//				pfc::string8 status = type;
	//				t_size space = type.find_first(" ");
	//				if (space != ~0)
	//				{
	//					status = type.get_ptr()+space+1;
	//					type.truncate(space);
	//				}
	//				mbr->setType(type);
	//				mbr->setStatus(status);
	//			}
	//		} catch (ticpp::Exception) {}
	//		try {
	//			ticpp::Iterator<ticpp::Element> release_event;
	//			for (release_event = release_event.begin(release->FirstChildElement("release-event-list")); release_event != release_event.end(); ++release_event)
	//			{
	//				pfc::string8 str = release_event->GetAttribute("date").data();
	//				if (strcmp(str, "") != 0 && str[0] != '0' && 
	//					(strcmp(mbr->getDate(), "") == 0 ||
	//					 strcmp(str, mbr->getDate()) < 0 ||
	//					 (strlen(str) > strlen(mbr->getDate()) && memcmp(str, mbr->getDate(), strlen(mbr->getDate())) == 0)
	//					 )
	//					)
	//				{
	//					if (cfg_short_date && str.length() > 4) str.truncate(4);
	//					mbr->setDate(str);
	//				}
	//			}
	//		} catch (ticpp::Exception) {}
	//	}

	//	// Adding tracks
	//	ticpp::Iterator<ticpp::Element> track;
	//	//try {
	//	for (track = track.begin(release->FirstChildElement("track-list")); track != track.end(); ++track)
	//	{
	//		pfc::string8 track_title(track->FirstChildElement("title")->FirstChild()->Value().data());
	//		if (cfg_no_feat)
	//		{
	//			regex rx("^(.+?)(\\s+\\(feat\\.\\s+.+\\))?$");
	//			cmatch res;
	//			regex_search(track_title.get_ptr(), res, rx);
	//			pfc::string8 tmp;
	//			tmp.set_string(res[1].first, res[1].second - res[1].first);
	//			track_title = tmp;
	//		}
	//		Track *mbt = mbr->addTrack(track_title.get_ptr(), track->GetAttribute("id").data());

	//		if (track->FirstChildElement("artist", false))
	//		{
	//			mbt->setArtist(track->FirstChildElement("artist")->FirstChildElement("name")->FirstChild()->Value().data());
	//			mbt->setArtistId(track->FirstChildElement("artist")->GetAttribute("id").data());
	//			mbr->va = true;
	//		}
	//	}
	//	//} catch (ticpp::Exception e) {
	//	//	if (release_number >= 0)
	//	//	{
	//	//		//throw e;
	//	//	}
	//	//}
	// }
}

// TODO: abort checks, progress
//void Query::perform() {
//	try {
//		pfc::string8 buffer;
//		get(buffer);
//		parse(buffer);
//		p_status.set_progress(p_status.progress_max / (mbc->getReleasesCount() + 1));
//		for (unsigned int i = 0; i < mbc->getReleasesCount(); i++)
//		{
//			Release *release = mbc->getRelease(i);
//			if (release->getTracksCount() == 0)
//			{
//				RequestURL request_url(release->getId());
//				request_url.AddParam("inc", "artist+tracks", false);
//				pfc::stringcvt::string_os_from_utf8 url_converter(request_url.GetURL());
//				get_parse_xml(url_converter, p_abort, i);
//				p_status.set_progress(p_status.progress_max / (mbc->getReleasesCount() + 1) * (1 + i));
//			}
//		}
//		PostMessage(tagger_dialog, WM_FOO_MB_UPDATE_RELEASES_LIST, 0, 0);
//		ShowWindow(tagger_dialog, SW_SHOW);
//	} catch (exception_foo_mb_connection_error e) {
//		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
//		popup_message::g_show(e.g_what(), COMPONENT_TITLE, popup_message::icon_error);
//	} catch (exception_aborted) {
//		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
//	} catch (exception_foo_mb_xml_parsing e) {
//		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
//		pfc::string8 error_sting = e.what();
//		popup_message::g_show(error_sting, COMPONENT_TITLE, popup_message::icon_error);
//	} catch (exception_foo_mb_no_releases e) {
//		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
//		popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
//	} catch (ticpp::Exception e) {
//		PostMessage(tagger_dialog, WM_CLOSE, 0, 0);
//		popup_message::g_show(e.what(), COMPONENT_TITLE, popup_message::icon_error);
//	}
//
//}


namespace foo_musicbrainz {
	Release *Query::get_release() {
		pfc::string8 buffer;
		get(buffer);
		ticpp::Document xml;
		parse(buffer, xml);

		return Parser::release(xml.FirstChildElement("metadata")->FirstChildElement("release"));
	}
}