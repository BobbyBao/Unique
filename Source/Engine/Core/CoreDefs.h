#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>

namespace Unique
{
	//template<class T>
	//using Vector = std::vector<T>;

	template<class T>
	using UPtr = std::unique_ptr<T>;

	template<class T>
	using List = std::list<T>;

	template<class K, class V>
	using Pair = std::pair<K, V>;

	template<class K, class V>
	using Map = std::map<K, V>;

	template<class K, class V>
	using HashMap = std::unordered_map<K, V>;

	template<class T>
	using HashSet = std::unordered_set<T>;
	
}