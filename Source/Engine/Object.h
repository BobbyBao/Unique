#pragma once
#include <memory>

class Object : public std::enable_shared_from_this<Object>
{
public:
	Object();
	~Object();
};

