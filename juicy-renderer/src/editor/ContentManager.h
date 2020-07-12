#pragma once

namespace JR {

class ContentManager {
public:
	ContentManager();
	void CreateNewContent(std::filesystem::path projectPath);
	void Load(std::filesystem::path projectPath);

	void ImportFile();

private:
	void ImportFile(std::filesystem::path importFile);

	bool mIsLoaded = false;

	std::filesystem::path mContentPath;

	std::unordered_map<std::size_t, std::string> mFileMap;

	MessageToken mDroppedFileToken;
};

}  // namespace JR