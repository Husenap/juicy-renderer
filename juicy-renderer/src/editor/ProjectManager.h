#pragma once

namespace JR {

class ProjectManager {
public:
	ProjectManager();
	bool IsLoaded() const { return mIsLoaded; }

	void SelectProject();

private:
	void CreateNewProject();
	
	void LoadProject();
	void LoadProject(std::filesystem::path projectFilePath);
	void SaveProject();

	bool mIsLoaded = false;

	MessageToken mKeyPressToken;
	MessageToken mDroppedFileToken;

	std::filesystem::path mProjectPath;
	std::filesystem::path mProjectFilePath;
};

}  // namespace JR
