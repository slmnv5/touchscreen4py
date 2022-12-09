#include "pch.hpp"
#include "lib/catch.hpp"
#include "TouchAndBuff.hpp"
#include "FrameBuff.hpp"
#include "TouchScr.hpp"

TEST_CASE("Test 1", "[all][basic]")
{

	SECTION("Section 1")
	{

		LOG(LogLvl::DEBUG) << "TEST0";
		LOG(LogLvl::INFO) << "TEST1";
		LOG(LogLvl::WARN) << "TEST2";
		LOG(LogLvl::ERROR) << "TEST3";

		REQUIRE(Log::ReportingLevel() == LogLvl::DEBUG);
		REQUIRE(static_cast<LogLvl>(2) == LogLvl::WARN);
		REQUIRE(Log::toString(static_cast<LogLvl>(3)) == "ERROR");
	}

	SECTION("Section 2")
	{
		LOG::ReportingLevel() = LogLvl::WARN;
		LOG(LogLvl::DEBUG) << "TEST0";
		LOG(LogLvl::INFO) << "TEST1";
		LOG(LogLvl::WARN) << "TEST2";
		LOG(LogLvl::ERROR) << "TEST3";
		LOG::ReportingLevel() = LogLvl::DEBUG;
	}
}

TEST_CASE("Test 2", "[all][basic]")
{

	SECTION("Section 1")
	{
		LOG(LogLvl::INFO) << "touch screen device: " << find_touchscr_event();
	}

	SECTION("Section 2")
	{
		FrameBuff fb;
		LOG(LogLvl::INFO) << "Frame buffer created";
		fb.drawSquare(122, 122, 50, 50, 222);
		LOG(LogLvl::INFO) << "Square draw done!!!";
		fb.drawSquare(33, 33, 11, 22, 2211);
		fb.drawSquare(77, 77, 33, 55, 65000);
	}
}
