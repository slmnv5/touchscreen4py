#include "pch.hpp"
#include "TouchAndBuff.hpp"

using namespace std;

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
	catch (exception &e)
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
	TouchScr ts(false, true);
	ts.run();
}
void test1()
{
	FrameBuff fb;
	std::string input;
	int x, y;

	while (true)
	{
		std::cout << "Enter X: ";
		getline(std::cin, input);
		if (!tryParse(input, x))
		{
			continue;
		}
		std::cout << "Enter Y: ";
		getline(std::cin, input);
		if (!tryParse(input, y))
		{
			continue;
		}

		fb.draw_square(x, y, 50, 50, COLOR_INDEX_T::GREEN);
	}
}

bool tryParse(std::string &input, int &output)
{
	try
	{
		output = std::stoi(input);
	}
	catch (std::invalid_argument)
	{
		return false;
	}
	return true;
}
