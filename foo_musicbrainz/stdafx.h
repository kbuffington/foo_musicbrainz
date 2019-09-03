#pragma once

#define _WIN32_WINNT 0x0501

#define COMPONENT_TITLE "MusicBrainz Tagger"
#define COMPONENT_DLL_NAME "foo_musicbrainz"
#define COMPONENT_VERSION "0.3.11"

#include <regex>

#include "resource.h"
#include "../foobar2000-SDK/foobar2000/helpers/foobar2000+atl.h"
#include "../foobar2000-SDK/foobar2000/helpers/atl-misc.h"
#include "../foobar2000-SDK/libPPUI/InPlaceEditTable.h"
#include "../foobar2000-SDK/libPPUI/listview_helper.h"
#include "mb_preferences.h"
#include "mb_list.h"
