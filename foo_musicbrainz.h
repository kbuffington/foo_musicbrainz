#ifndef __FOO_MUSICBRAINZ_H__
#define __FOO_MUSICBRAINZ_H__

#define TIXML_USE_TICPP
#define _WIN32_WINNT 0x0501

#define COMPONENT_TITLE "MusicBrainz Tagger"
#define COMPONENT_DLL_NAME "foo_musicbrainz"
#define COMPONENT_VERSION "0.0.1"

#include "discid.h"
#include "resource.h"
#include "../../SDKs/foobar2000/foobar2000/SDK/foobar2000.h"
#include "../../SDKs/foobar2000/foobar2000/ATLHelpers/ATLHelpers.h"
#include "../../SDKs/ticpp/ticpp.h"
#include <Winhttp.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../SDKs/SHA-1/sha1.h"
#include "discid.h"
#include "main.h"
#include "mbTrack.h"
#include "mbRelease.h"
#include "mbCollection.h"
#include "URLEncode.h"

#endif
