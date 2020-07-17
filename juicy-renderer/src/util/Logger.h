#pragma once

namespace JR {

class Logger : public Module {
public:
	Logger();

	enum class LogLevel { Debug, Info, Warning, Error, Fatal, Count };

	void SetLevel(LogLevel level) { mLevel = level; }

	template <typename... Args>
	void Log(LogLevel level,
	         const char* file,
	         uint32_t line,
	         const char* function,
	         const char* formatString,
	         Args&&... args) {
		InternalLog(level, file, line, function, fmt::format(formatString, std::forward<Args>(args)...));
	}

	static constexpr int DebugIdx   = 1;
	static constexpr int InfoIdx    = 2;
	static constexpr int WarningIdx = 3;
	static constexpr int ErrorIdx   = 4;
	static constexpr int FatalIdx   = 5;
	static constexpr int DarkIdx    = 7;

private:
	void InternalLog(LogLevel level, const char* file, uint32_t line, const char* function, const std::string& text);
	const char* GetLevelString(LogLevel level);

	LogLevel mLevel;
};

#ifdef _DEBUG

#	define LOG_DEBUG(...) MM::Get<Logger>().Log(Logger::LogLevel::Debug, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
#	define LOG_INFO(...) MM::Get<Logger>().Log(Logger::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
#	define LOG_WARNING(...) MM::Get<Logger>().Log(Logger::LogLevel::Warning, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
#	define LOG_ERROR(...) MM::Get<Logger>().Log(Logger::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
#	define LOG_FATAL(...) MM::Get<Logger>().Log(Logger::LogLevel::Fatal, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);

#else

#	define LOG_DEBUG(...)
#	define LOG_INFO(...)
#	define LOG_WARNING(...)
#	define LOG_ERROR(...)
#	define LOG_FATAL(...)

#endif

}  // namespace JR