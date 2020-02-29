#pragma once
#include "LruCache.cpp"

namespace mb {
	struct CacheObj {
		t_filetimestamp cached_time;
		str8 response;
	};

	class query_cache
	{
	public:
		query_cache() : cache(100) {}; // setting max size of cache to 100
		bool get(str8 url, str8* val);
		void set(str8 url, str8 response);

	private:
		int hashCode(str8 text);
		LruCache<int, CacheObj> cache;
	};
}
