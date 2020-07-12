#include "ProjectManager.h"

#include <juicy-fd/FileDialog.h>

#include "framework/Window.h"

namespace JR {

ProjectManager::ProjectManager() {
	mKeyPressToken = MM::Get<Window>().Subscribe<EventKeyPress>([&](const auto& e) {
		if (!mIsLoaded) {
			return;
		}
		if (e.mods == GLFW_MOD_CONTROL) {
			switch (e.key) {
			case GLFW_KEY_O:
				LoadProject();
				break;
			case GLFW_KEY_S:
				SaveProject();
				break;
			case GLFW_KEY_I:
				mContentManager.ImportFile();
				break;
			}
		}
	});
}

void ProjectManager::SelectProject() {
	if (!ImGui::IsPopupOpen("Select Project")) {
		ImGui::OpenPopup("Select Project");
	}
	if (ImGui::BeginPopupModal("Select Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		if (ImGui::Button("Create New", ImVec2(120, 0))) {
			CreateNewProject();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Existing", ImVec2(120, 0))) {
			LoadProject();
		}
		ImGui::EndPopup();
	}
}

void ProjectManager::CreateNewProject() {
	auto optProjectFilePath = JFD::FileDialog::SaveFile({L"jrproj"});
	if (!optProjectFilePath) {
		return;
	}

	auto projectFilePath = *optProjectFilePath;
	projectFilePath.replace_extension("jrproj");

	std::filesystem::path projectPath = projectFilePath;
	projectPath.remove_filename();

	// Create project folder
	std::filesystem::create_directories(projectPath);

	// Create project file
	{
		std::ofstream projectFile;
		projectFile.open(projectFilePath, std::ios_base::binary);
	}

	mContentManager.CreateNewContent(projectPath);

	LoadProject(projectFilePath);
}

void ProjectManager::LoadProject() {
	auto openFile = JFD::FileDialog::OpenFile({L"jrproj"});
	if (openFile) {
		LOG_INFO("Opening file: %ls", openFile->c_str());
		LoadProject(*openFile);
	}
}

void ProjectManager::LoadProject(std::filesystem::path projectFilePath) {
	if (!std::filesystem::exists(projectFilePath)) {
		mIsLoaded = false;
		return;
	}
	mProjectFilePath = projectFilePath;

	mProjectPath = mProjectFilePath;
	mProjectPath.remove_filename();

	mContentManager.Load(mProjectPath);

	mIsLoaded = true;
}

void ProjectManager::SaveProject() {
	auto saveFile = JFD::FileDialog::SaveFile({L"jrproj"});
	if (saveFile) {
		saveFile->replace_extension("jrproj");
		LOG_INFO("Saving file: %ls", saveFile->c_str());
	}
}

}  // namespace JR