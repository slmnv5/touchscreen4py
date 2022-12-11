#include "pch.hpp"
#include "lib/catch.hpp"
#include "FrameBuffer.hpp"
#include "TouchScreen.hpp"
#include "lib/utils.hpp"

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

TEST_CASE("Test FB 1", "[all][basic]")
{

	SECTION("Section 1a")
	{
		std::string word = wordAtPosition(" Test [once we] do", 11, '[', ']');
		LOG(LogLvl::INFO) << "====" << word << "========";
		assert(word == "[once we]");
	}

	SECTION("Section 1b")
	{
		std::string word = wordAtPosition(" Test [once we] do", 0, '[', ']');
		LOG(LogLvl::INFO) << "====" << word << "========";
		assert(word == "");
	}

	SECTION("Section 1c")
	{
		std::string word = wordAtPosition(" Test [once we] do", 11111, '[', ']');
		LOG(LogLvl::INFO) << "====" << word << "========";
		assert(word == "");
	}

	SECTION("Test FB squares, colors")
	{

		LOG(LogLvl::INFO) << "====================Frame buffer test============";
		FrameBuffer fb;
		LOG(LogLvl::INFO) << "Frame buffer created";
		fb.draw_square(12, 12, 15, 15, COLOR_INDEX_T::RED);
		LOG(LogLvl::INFO) << "Square draw done!!!";
		fb.clear();
		fb.draw_square(473, 233, 22, 22, COLOR_INDEX_T::WHITE);
		fb.draw_square(133, 310, 22, 22, COLOR_INDEX_T::BLUE);
		fb.draw_square(223, 233, 22, 22, COLOR_INDEX_T::RED);
		fb.draw_square(453, 13, 22, 22, COLOR_INDEX_T::YELLOW);
		sleep(2);
		assert(fb.mPixelsX > 10);
		assert(fb.mPixelsY > 10);
		assert(fb.mPixelsX < 10000);
		assert(fb.mPixelsY < 10000);
	}
}

TEST_CASE("Test FB 2", "[all][basic]")
{

	SECTION("Test text strings")
	{

		LOG(LogLvl::INFO) << "====================Frame buffer test============";
		FrameBuffer fb;
		fb.clear();
		fb.putString(120, 0, "ABACUS 121", COLOR_INDEX_T::GREEN);
		fb.putString(120, 0 + fb.mFont.height, "BIBIGON 321", COLOR_INDEX_T::RED);
		fb.putString(220, 220, "ABACUS 121=================>", COLOR_INDEX_T::YELLOW);
	}
}

TEST_CASE("Test TS 1", "[all][basic]")
{
	SECTION("Test click on TS")
	{
		LOG(LogLvl::INFO) << "Touch screen device: " << findTouchscrEvent();
		LOG(LogLvl::INFO) << "====================Frame buffer test============";
		TouchScreen ts(false, true);
		std::thread runThread(&TouchScreen::run, ts);
		for (int i = 0; i < 10; i++)
		{
			std::pair<int, int> pos = ts.getClickPosition();
			LOG(LogLvl::INFO) << "=============" << pos.first << ":" << pos.second;
		}
	}
}
