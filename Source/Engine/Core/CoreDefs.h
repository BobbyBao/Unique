#pragma once

#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>


namespace Unique
{
	typedef unsigned uint;
	typedef unsigned short ushort;
	typedef unsigned char byte;

	template<class T>
	using Vector = std::vector<T>;

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
	
	using String = std::string;
}