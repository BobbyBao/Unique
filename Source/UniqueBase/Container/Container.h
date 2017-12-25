#pragma once


#include <memory>
#include <functional>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>

namespace Unique
{
	template<class T>
	using Vector = std::vector<T>;

	template<class T>
	using PODVector = std::vector<T>;

	template<class T>
	using UPtr = std::unique_ptr<T>;

	template<class T>
	using List = std::list<T>;

	template<class K, class V>
	using Pair = std::pair<K, V>;

	template<class K, class V>
	using HashMap = std::unordered_map<K, V>;

	template<class T>
	using HashSet = std::unordered_set<T>;

	using ByteArray = Vector<char>;
}

#include "Container/ContainerUtils.h"
