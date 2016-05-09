#pragma once

#include "foo_musicbrainz.h"

namespace foo_musicbrainz {
	PFC_DECLARE_EXCEPTION(GenericException, pfc::exception, "Something went wrong.")
	PFC_DECLARE_EXCEPTION(XmlParseError, GenericException, "Error parsing XML.")
	PFC_DECLARE_EXCEPTION(NotFound, GenericException, "Nothing found.")
}
