#pragma once

#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER _WIN32_WINNT_WIN7

#include <regex>
using std::regex;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "resource.h"
#include <foobar2000/helpers/foobar2000+atl.h>
#include <foobar2000/helpers/atl-misc.h>
#include <libPPUI/CDialogResizeHelper.h>
#include <libPPUI/CListControlOwnerData.h>
#include <libPPUI/CListControlSimple.h>

using namespace pfc::stringcvt;
using str8 = pfc::string8;

#include "foo_musicbrainz.h"
#include "helpers.h"
#include "preferences.h"
