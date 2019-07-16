#pragma once

#define _WIN32_WINNT 0x0501

#define COMPONENT_TITLE "MusicBrainz Tagger"
#define COMPONENT_DLL_NAME "foo_musicbrainz"
#define COMPONENT_VERSION "0.3.10"

#include "resource.h"
#include "../foobar2000/ATLHelpers/ATLHelpersLean.h"
#include "../foobar2000/ATLHelpers/misc.h"
#include "../foobar2000/ATLHelpers/inplace_edit_v2.h"
#include "../tinyxml/tinyxml.h"
#include "meta.h"
#include "Preferences.h"
#include "exceptions.h"
