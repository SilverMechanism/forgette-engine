module;
export module logger;
import std;
import core;

export namespace logger
{
	enum class LogLevel : std::uint8_t
	{
		info,
		warning,
		error
	};
}

using namespace logger;

export
{
	void log(std::string text);
	void log(std::string text, LogLevel log_level);
}

void log(std::string text)
{
	log(text, LogLevel::info);
}

void log(std::string text, LogLevel log_level)
{
	switch (log_level)
	{
		case LogLevel::info:
			text = "[INFO] " + text;
			break;
		case LogLevel::warning:
			text = "[WARNING] " + text;
			break;
		case LogLevel::error:
			text = "[ERROR] " + text;
			break;
		default:
			text = "[INFO] " + text;
			break;
		
		std::cout << text << std::endl;
	}
}
