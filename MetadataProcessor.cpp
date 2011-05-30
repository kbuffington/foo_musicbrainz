#include <regex>
#include "foo_musicbrainz.h"
#include "MetadataProcessor.h"

using namespace std::tr1;

namespace foo_musicbrainz {
	// {2311DC1D-1A81-4496-9139-678481C813F8}
	const GUID MetadataProcessor::class_guid =
	{ 0x2311dc1d, 0x1a81, 0x4496, { 0x91, 0x39, 0x67, 0x84, 0x81, 0xc8, 0x13, 0xf8 } };

	void MetadataProcessor::apply_all(ReleaseList &release_list) {
		service_enum_t<MetadataProcessor> processors;
		service_ptr_t<MetadataProcessor> processor;
		while (processors.next(processor)) {
			if (!processor->is_enabled()) continue;
			apply_one(processor, release_list);
		}
	}

	void MetadataProcessor::apply_one(service_ptr_t<MetadataProcessor> processor, ReleaseList &release_list) {
		auto entities = processor->get_entities();

		bool process_release_list = entities & release_list_entity;
		bool process_release = entities & release_entity;
		bool process_medium = entities & medium_entity;
		bool process_track = entities & track_entity;

		if (entities < release_list_entity) return;
		bool after_release_list = entities > release_list_entity;
		bool after_release = entities > release_entity;
		bool after_medium = entities > medium_entity;

		if (process_release_list) {
			processor->process(release_list);
		}
		if (!after_release_list) return;

		for (size_t i = 0; i < release_list.count(); i++) {
			auto release = release_list[i];
			if (process_release) {
				processor->process(*release);
			}

			if (!after_release) continue;
			auto medium_list = release->get_medium_list();
			for (size_t j = 0; j < medium_list->count(); j++) {
				auto medium = medium_list->get(j);
				if (process_medium) {
					processor->process(*release);
				}

				if (!after_medium) continue;
				auto track_list = medium->get_track_list();
				for (size_t k = 0; k < track_list->count(); k++) {
					auto track = track_list->get(k);
					if (process_track) {
						processor->process(*track);
					}
				}
			}
		}
	}

	class RemoveFeatProcessor : public MetadataProcessor {
	public:
		Entities get_entities() { return track_entity; }
		bool is_enabled() { return Preferences::no_feat; }

		void process(Track &track) {
			auto recording = track.get_recording();
			auto title = recording->get_title();
			static regex rx("^(.+?)(\\s+\\(feat\\.\\s+.+\\))?$");
			cmatch matches;
			if (regex_search(title.get_ptr(), matches, rx)) {
				recording->set_title(matches[1].str().data());
			}
		}
	};

	service_factory_single_t<RemoveFeatProcessor> remove_feat_processor;
}
