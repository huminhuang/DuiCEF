#pragma once

#include <set>
#include <string>

namespace V8WindowFunciotn
{
#define DECLARE_WINDOWS_FUNCTION(name, function) static std::string name = insert(function);

	static std::set<std::string> Functions;
	static auto insert = [&](const char* function)
	{ 
		Functions.insert(function);

		return function;
	};

	// jsµ÷ÓÃwindow.test()
	DECLARE_WINDOWS_FUNCTION(winTest, "test");
}