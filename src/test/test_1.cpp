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
		fb.draw_square(12, 12, 15, 15, COLOR_INDEX_T::RED);
		LOG(LogLvl::INFO) << "Square draw done!!!";
		fb.clear();
		fb.draw_square(473, 233, 22, 22, COLOR_INDEX_T::WHITE);
		fb.draw_square(133, 310, 22, 22, COLOR_INDEX_T::BLUE);
		fb.draw_square(223, 233, 22, 22, COLOR_INDEX_T::RED);
		fb.draw_square(453, 13, 22, 22, COLOR_INDEX_T::YELLOW);
		sleep(5);
		assert(fb.res_x() > 10);
		assert(fb.res_y() > 10);
		assert(fb.res_x() < 10000);
		assert(fb.res_y() < 10000);
	}
}

TEST_CASE("Test 3", "[all][basic]")
{

	SECTION("Section 1")
	{

		LOG(LogLvl::INFO) << "====================Frame buffer test============";
		FrameBuff fb;
		fb.clear();
		fb.set_font(font_8x8);
		fb.put_char111(120, 120, 'A', COLOR_INDEX_T::GREEN);
		fb.set_font(font_16x32);
		fb.put_char111(220, 220, 'A', COLOR_INDEX_T::YELLOW);
	}
}
