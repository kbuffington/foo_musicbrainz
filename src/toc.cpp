#include "stdafx.h"
#include "toc.h"
#include "sha1.h"

namespace mb
{
	toc::toc(metadb_handle_list_cref handles) : num_tracks(handles.get_count()), tracks(100, 0)
	{
		regex rx("^\\d\\d:\\d\\d:\\d\\d$");
		const char* msf = handles.get_item(0)->get_info_ref()->info().info_get("pregap");
		if (msf != nullptr && regex_match(msf, rx))
		{
			pregap += (((msf[0] - '0') * 10 + (msf[1] - '0')) * 60 + (msf[3] - '0') * 10 + (msf[4] - '0')) * 75 + (msf[6] - '0') * 10 + (msf[7] - '0');
		}

		tracks[1] = pregap;

		const uint32_t bits = handles.get_item(0)->get_info_ref()->info().info_get_int("samplerate") == 48000 ? 640 : 588;
		uint32_t samples;

		for (uint32_t i = 0; i < num_tracks; i++)
		{
			samples = static_cast<uint32_t>(handles.get_item(i)->get_info_ref()->info().info_get_length_samples());
			const auto idx = i < num_tracks - 1 ? i + 2 : 0;
			tracks[idx] = tracks[i + 1] + (samples / bits);
		}
	}

	toc::~toc() {}

	str8 toc::get_discid()
	{
		if (discid.is_empty())
		{
			SHA1Context context;
			char tmp[9];

			sprintf_s(tmp, "%02X", 1);
			SHA1Input(&context, (uint8_t*)tmp, 2);

			sprintf_s(tmp, "%02X", (unsigned) num_tracks);
			SHA1Input(&context, (uint8_t*)tmp, 2);

			for (const auto track : tracks)
			{
				sprintf_s(tmp, "%08X", track);
				SHA1Input(&context, (uint8_t*)tmp, 8);
			}

			std::vector<uint8_t> digest(SHA1HashSize);
			SHA1Result(&context, digest.data());
			discid = rfc822_binary(digest);
		}
		return discid;
	}

	str8 toc::get_toc_url()
	{
		if (toc_url.is_empty())
		{
			toc_url << prefs::get_server() << "/cdtoc/attach?toc=1 " << num_tracks;
			for (uint32_t i = 0; i <= num_tracks; i++)
			{
				toc_url << " " << tracks[i];
			}
		}
		return toc_url;
	}

	str8 toc::rfc822_binary(std::vector<uint8_t>& src)
	{
		uint8_t* s = src.data();
		size_t srcl = SHA1HashSize;
		std::string ret;
		std::string v = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._";
		while (srcl)
		{
			ret += v[s[0] >> 2];
			ret += v[((s[0] << 4) + (--srcl ? (s[1] >> 4) : 0)) & 0x3f];
			ret += srcl ? v[((s[1] << 2) + (--srcl ? (s[2] >> 6) : 0)) & 0x3f] : '-';
			ret += srcl ? v[s[2] & 0x3f] : '-';
			if (srcl) srcl--;
			s += 3;
		}
		return ret.c_str();
	}
}
