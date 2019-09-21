#pragma once

#define _WIN32_WINNT _WIN32_WINNT_WINXP

#define COMPONENT_TITLE "MusicBrainz Tagger"
#define COMPONENT_DLL_NAME "foo_musicbrainz.dll"
#define COMPONENT_VERSION "0.4.0"

#include <regex>
using std::regex;

#include "json.hpp"
using json = nlohmann::json;

#include "resource.h"
#include <foobar2000/helpers/foobar2000+atl.h>
#include <foobar2000/helpers/atl-misc.h>
#include <libPPUI/CDialogResizeHelper.h>
#include <libPPUI/CListControlOwnerData.h>
#include <libPPUI/CListControlSimple.h>

using namespace pfc::stringcvt;
using str8 = pfc::string8;

#include "mb_helpers.h"
#include "mb_preferences.h"
