#pragma once

#define TIXML_USE_TICPP
#define _WIN32_WINNT 0x0501

#define COMPONENT_TITLE "MusicBrainz Tagger"
#define COMPONENT_DLL_NAME "foo_musicbrainz"
#define COMPONENT_VERSION "0.0.2"

#define WM_FOO_MB_UPDATE_RELEASES_LIST WM_USER + 0x100
#define WM_FOO_MB_UPDATE_RELEASE       WM_USER + 0x101

#include "discid.h"
#include "resource.h"
#include "../../SDKs/foobar2000/foobar2000/SDK/foobar2000.h"
#include "../../SDKs/foobar2000/foobar2000/ATLHelpers/ATLHelpers.h"
#include "../../SDKs/ticpp/ticpp.h"
#include <Winhttp.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../SDKs/SHA-1/sha1.h"
#include "list_view_edit.h"
#include "discid.h"
#include "mbTrack.h"
#include "mbRelease.h"
#include "mbCollection.h"
#include "URLEncode.h"
#include "TaggerDialogProc.h"
#include "foo_mb_menu.h"
#include "foo_mb_request_thread.h"
#include "uconvert.h"
#include "foo_mb_file_info_filter_impl.h"
