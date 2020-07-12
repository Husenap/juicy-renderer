#pragma once

#include "editor/ContentManager.h"

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

	ContentManager mContentManager;

	MessageToken mKeyPressToken;

	std::filesystem::path mProjectPath;
	std::filesystem::path mProjectFilePath;
};

}  // namespace JR
