
#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "myfun.hpp"


TEST_CASE("Test IsIrrelavant function", "[IsIrrelavant]") {
	SECTION("Simulation is not start, start_flag = 0, pause_flag = 0") {
		start_flag = 0; pause_flag = 0;

		REQUIRE(IsIrrelevant(kStart) == false);
		REQUIRE(IsIrrelevant(kPause) == true);
		REQUIRE(IsIrrelevant(kStop) == true);
		REQUIRE(IsIrrelevant(kContinue) == true);
		REQUIRE(IsIrrelevant(kUnknow) == true);
	}

	SECTION("Simulation is not start, start_flag = 0, pause_flag = 1") {
		start_flag = 0; pause_flag = 1;

		REQUIRE(IsIrrelevant(kStart) == true);
		REQUIRE(IsIrrelevant(kPause) == true);
		REQUIRE(IsIrrelevant(kStop) == true);
		REQUIRE(IsIrrelevant(kContinue) == true);
		REQUIRE(IsIrrelevant(kUnknow) == true);
	}

	SECTION("Simulation has been started, start_flag = 1, pause_flag = 0") {
		start_flag = 1; pause_flag = 0;

		REQUIRE(IsIrrelevant(kStart) == true);
		REQUIRE(IsIrrelevant(kPause) == false);
		REQUIRE(IsIrrelevant(kStop) == false);
		REQUIRE(IsIrrelevant(kContinue) == true);
		REQUIRE(IsIrrelevant(kUnknow) == true);
	}

	SECTION("Simulation has been started, start_flag = 1, pause_flag = 1") {
		start_flag = 1; pause_flag = 1;

		REQUIRE(IsIrrelevant(kStart) == true);
		REQUIRE(IsIrrelevant(kPause) == true);
		REQUIRE(IsIrrelevant(kStop) == false);
		REQUIRE(IsIrrelevant(kContinue) == false);
		REQUIRE(IsIrrelevant(kUnknow) == true);
	}

}
