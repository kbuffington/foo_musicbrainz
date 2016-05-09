#include "foo_musicbrainz.h"
#include "MetadataProcessor.h"

namespace foo_musicbrainz {
	class AsciiPunctuation : public MetadataProcessor {
		unsigned int get_entities() { return track_entity | release_entity; }
		bool is_enabled() { return Preferences::ascii_punctuation; }

		void replace(pfc::string8 &str) {
			pfc::string tmp(str);
			for (size_t i = 0; i < replacement_size; i++) {
				auto what = replacements[i][0];
				auto with = replacements[i][1];
				tmp = tmp.replace(what, with);
			}
			str = tmp.get_ptr();
		}

		void process(Release &release) {
			auto tmp = release.get_title();
			replace(tmp);
			release.set_title(tmp);
		}

		void process(Track &track) {
			auto tmp = track.get_title();
			replace(tmp);
			track.set_title(tmp);
		}

		static const char *replacements[][2];
		static const int replacement_size;
	};

	// Only to be used with string literals — causes memory leaks.
	# define u(str) utf8(L##str)
	const char *utf8(const wchar_t *str) {
		pfc::stringcvt::string_utf8_from_wide converted(str);
		auto tmp = new char[converted.length() + 1];
		strcpy(tmp, converted.get_ptr());
		return tmp;
	}

	const char *AsciiPunctuation::replacements[][2] = {
		{ u("…"), u("...") },
		{ u("‘"), u("'") },
		{ u("’"), u("'") },
		{ u("‚"), u("'") },
		{ u("“"), u("\"") },
		{ u("”"), u("\"") },
		{ u("„"), u("\"") },
		{ u("′"), u("'") },
		{ u("″"), u("\"") },
		{ u("‹"), u("<") },
		{ u("›"), u(">") },
		{ u("«"), u("\"") },
		{ u("»"), u("\"") },
		{ u("‐"), u("-") },
		{ u("‒"), u("-") },
		{ u("–"), u("-") },
		{ u("−"), u("-") },
		{ u("—"), u("-") },
		{ u("―"), u("-") }
	};
	const int AsciiPunctuation::replacement_size = sizeof(AsciiPunctuation::replacements) / sizeof(const char *) / 2;

	service_factory_single_t<AsciiPunctuation> metadata_processor;
}
