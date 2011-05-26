#pragma once

#define TIXML_USE_TICPP
#define _WIN32_WINNT 0x0501

#define COMPONENT_TITLE "MusicBrainz Tagger"
#define COMPONENT_DLL_NAME "foo_musicbrainz"
#define COMPONENT_VERSION "0.3"

#define WM_FOO_MB_UPDATE_RELEASES_LIST WM_USER + 0x100
#define WM_FOO_MB_UPDATE_RELEASE       WM_USER + 0x101

#include "resource.h"
#include "../foobar2000/SDK/foobar2000.h"
#include "../foobar2000/ATLHelpers/ATLHelpers.h"
#include "../ticpp/ticpp.h"
#include <Winhttp.h>
#include <stdlib.h>
#include <stdio.h>
#include "meta.h"
#include "TOC.h"
#include "Track.h"
#include "Release.h"
#include "ReleaseList.h"
#include "RequestURL.h"
#include "list_view_edit.h"
#include "foo_mb_file_info_filter_impl.h"
#include "CTaggerDialog.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_connection_error, pfc::exception, "Error connecting to musicbrainz.org.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_xml_parsing, pfc::exception, "Error parsing XML.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_no_releases, pfc::exception, "No releases.")
