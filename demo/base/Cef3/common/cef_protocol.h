#pragma once

#ifndef _CLIENT_PROTOCOL_H_
#define _CLIENT_PROTOCOL_H_

#include <set>
#include <string>

/*
	规范Cef Protocol写法
*/

namespace CefProtocol
{
	typedef const char* FunType;

	static std::set<FunType> fun_name_set;

	static auto addFun = [](IN FunType funName, IN std::set<FunType>& set)->FunType
	{	
		ASSERT(set.find(funName) == set.end());//插入的函数名已存在，报错

		set.insert(funName); return funName;	
	};


	//定义函数名，该函数名会绑定到v8引擎中，
	//对页面来说，相当于新增一个调用c++的函数接口
	//
	//
#define AddFunName(name /*函数别名*/ , fun /*函数名*/ ) \
	static FunType name = addFun(fun, fun_name_set);



	/*			AddFunName  begin			*/

	AddFunName(kTestFun	, "testFun");

	/*			AddFunName  end				*/




};//namespace CefProtocol

#endif //_CLIENT_PROTOCOL_H_