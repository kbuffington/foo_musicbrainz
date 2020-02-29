#pragma once

#include "stdafx.h"
#include <list>
#include <unordered_map>

using namespace std;

template <typename K, typename V>
class LruCache {
public:
	LruCache(unsigned int capacity) : capacity_(capacity) {};
	void set(const K& k, const V& v);
	bool get(const K& k, V& v);
	bool erase(const K& k);
	void setCacheSize(unsigned int capacity) { capacity_ = capacity; };
	int getCacheSize();
private:
	unsigned int capacity_;
	std::list<std::pair <K, V>> cacheList;
	using list_iterator = typename std::list <std::pair <K, V>> ::iterator;
	std::unordered_map <K, list_iterator>cacheMap;
};

template <typename K, typename V>
int LruCache<K, V>::getCacheSize() {
	int cacheSize = cacheList.size();

	return cacheSize;
}

template <typename K, typename V>
void LruCache <K, V>::set(const K& k, const V& v) {
	auto itr = cacheMap.find(k);

	if (itr != cacheMap.end()) {
		itr->second->second = v;
		cacheList.splice(cacheList.begin(), cacheList, itr->second);
		itr->second = cacheList.begin();
	}
	else {
		while (cacheMap.size() >= capacity_) {
			cacheMap.erase(cacheList.back().first);
			cacheList.pop_back();
		}
		cacheList.emplace_front(k, v);
		cacheMap.emplace(k, cacheList.begin());
	}
}

template <typename K, typename V>
bool LruCache <K, V>::get(const K& k, V& v) {
	auto itr = cacheMap.find(k);
	bool ret = false;

	if (itr != cacheMap.end()) {
		cacheList.splice(cacheList.begin(), cacheList, itr->second);
		itr->second = cacheList.begin(); // update iterator
		v = itr->second->second;
		ret = true;
	}

	return ret;
}

template <typename K, typename V>
bool LruCache <K, V>::erase(const K& k) {
	auto itr = cacheMap.find(k);
	bool ret = false;

	if (itr != cacheMap.end()) {
		cacheList.erase(itr->second);
		cacheMap.erase(itr);
		ret = true;
	}

	return ret;
}
