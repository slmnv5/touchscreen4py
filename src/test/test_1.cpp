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
		LOG(LogLvl::INFO) << "Touch screen device: " << find_touchscr_event();
	}

	SECTION("Section 2")
	{
		LOG(LogLvl::INFO) << "====================Frame buffer test============";
		FrameBuff fb;
		LOG(LogLvl::INFO) << "Frame buffer created";
		fb.drawSquare(12, 12, 15, 15, COLOR_INDEX_T::RED);
		LOG(LogLvl::INFO) << "Square draw done!!!";
		fb.drawSquare(473, 233, 22, 22, COLOR_INDEX_T::WHITE);
		fb.drawSquare(133, 310, 22, 22, COLOR_INDEX_T::BLUE);
		fb.drawSquare(223, 233, 22, 22, COLOR_INDEX_T::RED);
		fb.drawSquare(453, 13, 22, 22, COLOR_INDEX_T::YELLOW);

		assert(fb.resx() > 10);
		assert(fb.resy() > 10);

		LOG(LogLvl::INFO) << "====================Touch screen test============";
		TouchScr ts(true, false, true);
		LOG(LogLvl::INFO) << "Touch screen created";
		ts.run();
		LOG(LogLvl::INFO) << "Screen run done!!!";
	}
}
