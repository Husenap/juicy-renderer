#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace JFD {

class FileDialog {
public:
	static std::optional<std::filesystem::path> OpenFile(std::vector<std::wstring> extensionFilter = {});
	static std::optional<std::filesystem::path> SaveFile(std::vector<std::wstring> extensionFilter = {});

private:
	FileDialog()  = delete;
	~FileDialog() = delete;
};

}  // namespace JFD