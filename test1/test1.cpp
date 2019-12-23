// test1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

////#include "pch.h"
//#include <iostream>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "myfun.hpp"


TEST_CASE("Test IsIrrelavant function", "[IsIrrelavant]") {
	SECTION("start_flag = 0, pause_flag = 0") {
		start_flag = 0; pause_flag = 0;

		REQUIRE(IsIrrelevant(kStart) == false);
		REQUIRE(IsIrrelevant(kPause) == true);
		REQUIRE(IsIrrelevant(kStop) == true);
		REQUIRE(IsIrrelevant(kContinue) == true);
		REQUIRE(IsIrrelevant(kUnknow) == true);
	}

}
