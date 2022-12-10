#include "pch.hpp"
#include "lib/catch.hpp"
#include "TouchAndBuff.hpp"
#include "FrameBuff.hpp"
#include "TouchScr.hpp"

TEST_CASE("Test 1", "[all][basic]")
{

	SECTION("Section 1")
	{

		LOG(LogLvl::DEBUG) << "DEBUG";
		LOG(LogLvl::INFO) << "INFO";
		LOG(LogLvl::WARN) << "WARN";
		LOG(LogLvl::ERROR) << "ERROR";

		REQUIRE(Log::ReportingLevel() == LogLvl::DEBUG);
		REQUIRE(static_cast<LogLvl>(2) == LogLvl::WARN);
		REQUIRE(Log::toString(static_cast<LogLvl>(3)) == "ERROR");
	}

	SECTION("Section 2")
	{
		LOG::ReportingLevel() = LogLvl::WARN;
		LOG(LogLvl::DEBUG) << "Should not see this !!!!!!!";
		LOG(LogLvl::INFO) << "Should not see this !!!!!!!";
		LOG(LogLvl::WARN) << "WARN";
		LOG(LogLvl::ERROR) << "ERROR";
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
		fb.clear();
		fb.drawSquare(473, 233, 22, 22, COLOR_INDEX_T::WHITE);
		fb.drawSquare(133, 310, 22, 22, COLOR_INDEX_T::BLUE);
		fb.drawSquare(223, 233, 22, 22, COLOR_INDEX_T::RED);
		fb.drawSquare(453, 13, 22, 22, COLOR_INDEX_T::YELLOW);
		sleep(5);
		assert(fb.resx() > 10);
		assert(fb.resy() > 10);
	}
}

TEST_CASE("Test 3", "[all][basic]")
{

	SECTION("Section 1")
	{

		LOG(LogLvl::INFO) << "====================Frame buffer test============";
		FrameBuff fb;
		fb.clear();
		fb.put_string(120, 120, "AA", GREEN);
		fb.put_string(220, 220, "BB", COLOR_INDEX_T::YELLOW);
	}
}
