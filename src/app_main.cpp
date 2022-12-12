#include "pch.hpp"
#include "TouchScreenPy.hpp"

using myclock = std::chrono::steady_clock;
using seconds = std::chrono::duration<double>;
void help();
bool tryParse(std::string &, int &);
void test1();
void test2();

int main(int argc, char *argv[])
{

	LOG::ReportingLevel() = LogLvl::ERROR;

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-v") == 0)
		{
			LOG::ReportingLevel() = LogLvl::WARN;
		}
		else if (strcmp(argv[i], "-vv") == 0)
		{
			LOG::ReportingLevel() = LogLvl::INFO;
		}
		else if (strcmp(argv[i], "-vvv") == 0)
		{
			LOG::ReportingLevel() = LogLvl::DEBUG;
		}
		else if (strcmp(argv[i], "-h") == 0)
		{
			help();
			return 0;
		}
	}

	try
	{
		test2();
	}
	catch (std::exception &e)
	{
		LOG(LogLvl::ERROR) << "Completed with error: " << e.what();
		return 1;
	}
}

void help()
{
	cout << "Usage: mimap5 -r <file> [options] \n"
			"  -k <kbdMapFile> use typing keyboard for MIDI notes, needs sudo\n"
			"  -n [name] output MIDI port name to create\n"
			"  -v verbose output\n"
			"  -vv more verbose\n"
			"  -vvv even more verbose\n"
			"  -h displays this info\n";
}

void test2()
{
	TouchScreenPy tsp;
	tsp.mFrameBuffer.clear();
	tsp.setLoopSeconds(21.0, 0.55);
	tsp.setText("Here we have [some cool] stuff\nmay be [coming] soon [sooner]\nNo one is upset");

	auto started = myclock::now();
	seconds duration(0);
	while (duration.count() < 220)
	{
		LOG(LogLvl::INFO) << "Running duration: " << duration.count();
		duration = myclock::now() - started;
		auto word = tsp.getClickEvent();
		LOG(LogLvl::INFO) << "Got clickEvent: " << word;
	}
}

bool tryParse(std::string &input, int &output)
{
	try
	{
		output = std::stoi(input);
	}
	catch (std::invalid_argument &e)
	{
		return false;
	}
	return true;
}
