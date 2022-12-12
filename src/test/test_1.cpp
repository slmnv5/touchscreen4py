#include "pch.hpp"
#include "lib/catch.hpp"
#include "FrameBuffer.hpp"
#include "TouchScreen.hpp"
#include "TouchScreenPy.hpp"

#include "lib/log.hpp"
using myclock = std::chrono::steady_clock;
using seconds = std::chrono::duration<double>;

TEST_CASE("Test simple things", "[all][basic]")
{

	SECTION("Test all levels")
	{
		LOG(LogLvl::INFO) << "Touch screen device: " << findTouchscrEvent();

		LOG(LogLvl::DEBUG) << "DEBUG";
		LOG(LogLvl::INFO) << "INFO";
		LOG(LogLvl::WARN) << "WARN";
		LOG(LogLvl::ERROR) << "ERROR";

		REQUIRE(Log::ReportingLevel() == LogLvl::DEBUG);
		REQUIRE(static_cast<LogLvl>(2) == LogLvl::WARN);
		REQUIRE(Log::toString(static_cast<LogLvl>(3)) == "ERROR");
	}

	SECTION("Test some levels")
	{
		LOG::ReportingLevel() = LogLvl::WARN;
		LOG(LogLvl::DEBUG) << "!!!!!!!!!!!!!! Should not see this !!!!!!!";
		LOG(LogLvl::INFO) << "!!!!!!!!!!!!!!! Should not see this !!!!!!!";
		LOG(LogLvl::WARN) << "WARN";
		LOG(LogLvl::ERROR) << "ERROR";
		LOG::ReportingLevel() = LogLvl::DEBUG;
	}
}

TEST_CASE("Test FB 1", "[all][basic]")
{

	SECTION("Find word at position")
	{
		std::string word;
		word = wordAtPosition(" Test [once we] do", 11, '[', ']');
		LOG(LogLvl::INFO) << "====" << word << "========";
		assert(word == "[once we]");
		word = wordAtPosition(" Test [once we] do", 0, '[', ']');
		LOG(LogLvl::INFO) << "====" << word << "========";
		assert(word == "");
		word = wordAtPosition(" Test [once we] do", 11111, '[', ']');
		LOG(LogLvl::INFO) << "====" << word << "========";
		assert(word == "");
	}

	SECTION("Test FB squares, colors")
	{

		LOG(LogLvl::INFO) << "====================Frame buffer test============";
		FrameBuffer fb;
		LOG(LogLvl::INFO) << "Frame buffer created";
		fb.putSquare(12, 12, 15, 15, COLOR_INDEX::RED);
		LOG(LogLvl::INFO) << "Square draw done!!!";
		fb.clear();
		fb.putSquare(473, 233, 22, 22, COLOR_INDEX::WHITE);
		fb.putSquare(133, 310, 22, 22, COLOR_INDEX::BLUE);
		fb.putSquare(223, 233, 22, 22, COLOR_INDEX::RED);
		fb.putSquare(453, 13, 22, 22, COLOR_INDEX::YELLOW);
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
		fb.putString(120, 0, "ABACUS 121", COLOR_INDEX::GREEN);
		fb.putString(120, 0 + fb.mFont.height, "BIBIGON 321", COLOR_INDEX::RED);
		fb.putString(220, 220, "ABACUS 121=================>", COLOR_INDEX::YELLOW);
	}
}
