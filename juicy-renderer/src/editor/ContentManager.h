#pragma once

namespace JR {

class ContentManager : public Module {
public:
	void CreateNewContent(std::filesystem::path projectPath);
	void Load(std::filesystem::path projectPath);

	void ImportFile();
	void ImportFile(std::filesystem::path importFile);

	std::optional<std::filesystem::path> GetPath(StringId id) const;

	std::filesystem::path GetRelativePath(std::filesystem::path path) const {
		return path.lexically_relative(mContentPath);
	}

	std::filesystem::path GetContentPath() const { return mContentPath; }

private:
	std::filesystem::path mContentPath;

	std::map<StringId, std::filesystem::path> mFileMap;
};

}  // namespace JR