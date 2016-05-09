#include "foo_musicbrainz.h"
#include "TagWriter.h"
#include "FileTagMap.h"
#include "Track.h"

using namespace foo_musicbrainz;

TagWriter::TagWriter(FileTagMap files)
	: files(files)
{
}

bool TagWriter::apply_filter(metadb_handle_ptr p_location, t_filestats p_stats, file_info & p_info) {
	try {
		auto tags = files[p_location];
		for (auto tag = tags.first(); tag.is_valid(); tag++) {
			// TODO: return false if all info is identical
			p_info.meta_remove_field(tag->m_key);
			// TODO: text multivalues
			tag->m_value.for_each([&](pfc::string8 &value) {
				if (!value.is_empty()) {
					p_info.meta_add(tag->m_key, value);
				}
			});
		}
		return true;
	} catch (pfc::exception_map_entry_not_found) {
		return false;
	}
}
