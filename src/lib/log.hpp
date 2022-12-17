#ifndef __LOG1_H__
#define __LOG1_H__

#include <iostream>

enum class LogLvl
{
	DEBUG,
	INFO,
	WARN,
	ERROR
};

class Log
{
public:
	Log();
	virtual ~Log();
	std::ostream &Get(LogLvl level = LogLvl::INFO);

public:
	static LogLvl &ReportingLevel();
	static std::string toString(LogLvl level);
	static LogLvl FromString(const std::string &level);

protected:
private:
	Log(const Log &);
	Log &operator=(const Log &);
};

inline Log::Log()
{
}

inline std::ostream &Log::Get(LogLvl level)
{
	std::cerr << toString(level) << ": ";
	return std::cerr;
}

inline Log::~Log()
{
	std::cerr << std::endl;
	std::cerr.flush();
}

inline LogLvl &Log::ReportingLevel()
{
	static LogLvl reportingLevel = LogLvl::DEBUG;
	return reportingLevel;
}

inline std::string Log::toString(LogLvl level)
{
	static const char *const buffer[] = {"DEBUG", "INFO", "WARN", "ERROR"};
	return buffer[(int)level];
}

typedef Log LOG;

#define LOG(level)                     \
	if (level < LOG::ReportingLevel()) \
		;                              \
	else                               \
		Log().Get(level)

#endif
