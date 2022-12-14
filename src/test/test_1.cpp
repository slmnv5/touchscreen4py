#include "pch.hpp"
#include "lib/catch.hpp"
// #include "FrameBuffer.hpp"
// #include "TouchScreen.hpp"
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

	std::string s = " Test [once we] do ";
	SECTION("Find word at position 1")
	{
		std::string word;
		word = wordAtPosition(s, 11);
		assert(word == "[once we]");
		word = wordAtPosition(s, 0);
		assert(word == "");
		word = wordAtPosition(s, 11111);
		assert(word == "");
		word = wordAtPosition(s, 3);
		assert(word == "");
	}
}

TEST_CASE("Test FB 1", "[all]")
{

	SECTION("Test FB squares, colors")
	{

		FrameBuffer fb;
		LOG(LogLvl::INFO) << "Frame buffer created";
		fb.putSquare(12, 12, 15, 15, idxToColor(RED));
		LOG(LogLvl::INFO) << "Square draw done!!!";

		fb.putSquare(473, 233, 22, 22, idxToColor(WHITE));
		sleep(2);
		fb.clearScreen();
		fb.putSquare(133, 310, 22, 22, idxToColor(BLUE));
		sleep(2);
		fb.clearScreen();
		fb.putSquare(223, 233, 22, 22, idxToColor(RED));
		sleep(2);
		fb.clearScreen();
		fb.putSquare(453, 13, 22, 22, idxToColor(YELLOW));
		sleep(2);
		fb.clearScreen();
	}
}

TEST_CASE("Test FB 2", "[all]")
{

	SECTION("Test text strings")
	{

		FrameBuffer fb;
		fb.setRowText(2, "ABACUS 121 SSSSSSSSSSSSSSSSS AAAAAAAAAAAAA DD FFF FFFFFFFFF YYY", 100, 100, 100);
		fb.setRowText(5, "BIBIGON 321 AA HHHH JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ ", 100, 0, 0);
		fb.setRowText(220, "ABACUS 121 ~~~~~~~~~~~~~~~~~~>", 0, 100, 100);
	}
}