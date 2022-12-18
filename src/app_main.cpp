#include "pch.hpp"
#include "TouchScreenPy.hpp"

using myclock = std::chrono::steady_clock;
using seconds = std::chrono::duration<double>;
void help();

int main(int argc, char *argv[])
{

	LOG::ReportingLevel() = LogLvl::ERROR;
	const char *fbId = nullptr;

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
		else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
		{
			fbId = argv[i + 1];
		}
		else if (strcmp(argv[i], "-h") == 0)
		{
			help();
			return 0;
		}
	}

	try
	{
		uint fbIdInt = 0;
		if (nullptr != fbId)
		{
			fbIdInt = std::stoi(fbId);
		}
		TouchScreenPy tsp(fbIdInt);
		sleep(1);
		tsp.setLoop(21.0, 0.55, true, false);
		sleep(2);
		tsp.setText("Here we have [some cool]", 2, 0, 100, 100, 100);
		tsp.setText("May be [coming] soon or [sooner]", 4, 0, 100, 100, 100);
		sleep(2);
		LOG(LogLvl::INFO) << "Set text done";

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
	catch (std::exception &e)
	{
		LOG(LogLvl::ERROR) << "Completed with error: " << e.what();
		return 1;
	}
}

void help()
{
	std::cout << "Usage: touchscr5 [options] \n"
				 "  -f [fb Id] frame buffer Id to use\n"
				 "  -v verbose output\n"
				 "  -vv more verbose\n"
				 "  -vvv even more verbose\n"
				 "  -h displays this info\n";
}
