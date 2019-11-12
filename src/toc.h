#pragma once

namespace mb
{
	class toc
	{
	public:
		toc(metadb_handle_list_cref p_data);
		~toc();

		char* get_discid();
		const char* get_toc();

	private:
		unsigned int* tracks_lengths;
		unsigned int tracks[100];
		t_size num_tracks;
		t_size cur_track;
		unsigned int pregap;
		char* discid;
		str8 toc_str;
		void calculate_tracks();
		void set_pregap(str8 msf);
		unsigned char* rfc822_binary(void* src, unsigned long srcl, unsigned long& len);
	};
}
