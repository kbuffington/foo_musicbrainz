#pragma once

namespace mb
{
	class toc
	{
	public:
		toc(metadb_handle_list_cref handles);
		~toc();

		str8 get_discid();
		str8 get_toc_url();

	private:
		str8 rfc822_binary(std::vector<uint8_t>& src);

		std::vector<uint32_t> tracks;
		str8 discid;
		str8 toc_url;
		size_t num_tracks = 0;
		uint32_t pregap = 150;
	};
}
