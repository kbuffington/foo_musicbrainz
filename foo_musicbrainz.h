#pragma once

#define TIXML_USE_TICPP
#define _WIN32_WINNT 0x0501

#define COMPONENT_TITLE "MusicBrainz Tagger"
#define COMPONENT_DLL_NAME "foo_musicbrainz"
#define COMPONENT_VERSION "0.3"

#include "resource.h"
#include "../foobar2000/SDK/foobar2000.h"
#include "../foobar2000/ATLHelpers/ATLHelpers.h"
#include "../ticpp/ticpp.h"
#include "meta.h"
#include "Preferences.h"

PFC_DECLARE_EXCEPTION(exception_foo_mb_connection_error, pfc::exception, "Error connecting to musicbrainz.org.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_xml_parsing, pfc::exception, "Error parsing XML.")
PFC_DECLARE_EXCEPTION(exception_foo_mb_no_releases, pfc::exception, "No releases.")
