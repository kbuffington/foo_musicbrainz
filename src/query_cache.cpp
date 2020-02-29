#include "stdafx.h"
#include "query_cache.h"

// cache entries expire after 1 hour
#define CACHE_EXPIRATION	3600

namespace mb {
	
	bool query_cache::get(str8 url, str8* val)
	{
		int hash = hashCode(url);
		CacheObj cache_obj;

		bool hit = cache.get(hash, cache_obj);
		if (hit) {
			if (pfc::fileTimeWtoU(cache_obj.cached_time) > pfc::fileTimeWtoU(pfc::fileTimeNow()) - CACHE_EXPIRATION) {
				*val = cache_obj.response;
			}
			else {
				// stale
				hit = false;
			}
		}
		return hit;
	}

	void query_cache::set(str8 url, str8 response)
	{
		int hash = hashCode(url);
		CacheObj cache_obj;

		cache_obj.cached_time = pfc::fileTimeNow();
		cache_obj.response = response;
		
		cache.set(hash, cache_obj);
	}
	
	int query_cache::hashCode(str8 text) {
		int hash = 0, strlen = text.length(), i;
		char character;
		if (strlen == 0)
			return hash;
		for (i = 0; i < strlen; i++) {
			character = text[i];
			hash = (31 * hash) + (character);
		}
		return hash;
	}

}
