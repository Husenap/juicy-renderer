#include "Logger.h"

#include <cstdarg>

namespace JR {

Logger::Logger() {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	_CONSOLE_SCREEN_BUFFER_INFOEX csbi;
	csbi.cbSize = sizeof(_CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(console, &csbi);

	csbi.ColorTable[DebugIdx]   = 0x9BC8FC;
	csbi.ColorTable[DarkIdx]    = 0x606060;
	csbi.ColorTable[InfoIdx]    = 0x81D5AE;
	csbi.ColorTable[WarningIdx] = 0x4DB7FF;
	csbi.ColorTable[ErrorIdx]   = 0xA25FFF;
	csbi.ColorTable[FatalIdx]   = 0xDAA89F;

	SetConsoleScreenBufferInfoEx(console, &csbi);

	HWND hConsole = GetConsoleWindow();
	SetConsoleTitle("Juicy Console");
	MoveWindow(hConsole, 0, 0, 1250, 600, true);

	mBuffer.resize(8192);
}

void Logger::Log(LogLevel level, const char* file, uint32_t line, const char* function, const char* format, ...) {
	if (level < mLevel) {
		return;
	}

	std::string filename = std::filesystem::path(file).filename().string();

	std::string functionName = function;
	if (functionName.find("lambda") != std::string::npos) {
		functionName = functionName.substr(0, functionName.find("lambda") + 6) + ">";
	}
	if (std::size_t pos = functionName.find_last_of(":"); pos != std::string::npos) {
		functionName = functionName.substr(pos + 1);
	}

	va_list args;
	va_start(args, format);
	vsprintf_s(mBuffer.data(), mBuffer.size(), format, args);
	va_end(args);

	if (level >= LogLevel::Error) {
		HWND hConsole = GetConsoleWindow();
		FlashWindow(hConsole, false);
	}

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, DarkIdx);
	printf("[");
	SetConsoleTextAttribute(console, static_cast<WORD>(level)+1);
	printf("%s", GetLevelString(level));
	SetConsoleTextAttribute(console, DarkIdx);
	printf("]: %s:%d:%s: ", filename.c_str(), line, functionName.c_str());
	SetConsoleTextAttribute(console, DebugIdx);
	printf("%s\n", mBuffer.data());

	if (level == LogLevel::Fatal){
		throw std::runtime_error(mBuffer.data());
	}
}

const char* Logger::GetLevelString(LogLevel level) {
	constexpr static std::array<const char*, static_cast<std::size_t>(LogLevel::Count)> Strings{
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR",
		"FATAL",
	};
	
	return Strings[static_cast<std::size_t>(level)];
}

}  // namespace JR