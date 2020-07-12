#include "Editor.h"

#include "EditorUtil.h"
#include "components/Components.h"
#include "framework/Framework.h"
#include "framework/Window.h"

namespace JR {

using namespace Components;

Editor::Editor(ECS& ecs)
    : mECS(ecs)
    , mInspector(ecs)
    , mHierarchy(ecs) {
	mKeyPressToken = MM::Get<Window>().Subscribe<EventKeyPress>([&](const auto& e) {
		if (!mProjectManager.IsLoaded()) {
			return;
		}
		switch (e.key) {
		case GLFW_KEY_F1:
			mInspector.ToggleVisibility();
			break;
		case GLFW_KEY_F2:
			mHierarchy.ToggleVisibility();
			break;
		case GLFW_KEY_F3:
			mContentBrowser.ToggleVisibility();
			break;
		case GLFW_KEY_F4:
			mHistory.ToggleVisibility();
			break;
		case GLFW_KEY_F11:
			mShowEditor = !mShowEditor;
			break;
		}
	});

	mTransactionToken = MM::Get<TransactionManager>().Subscribe<EventTransaction>([&](const EventTransaction& message) {
		auto it = mTransactionHandlers.find(message.componentId);
		if (it == mTransactionHandlers.end()) {
			LOG_ERROR("Failed to find transaction handler for component with id: %d!", message.componentId);
			return;
		}

		it->second(message);
	});
}

void Editor::Update() {
	if (!mProjectManager.IsLoaded()) {
		mProjectManager.SelectProject();
		return;
	}

	if (!mShowEditor) {
		return;
	}

	DrawDockSpace();
	mInspector.Update();
	mHistory.Update();
	mHierarchy.Update();
	mContentBrowser.Update();

	static bool demo = true;
	if (demo) {
		ImGui::ShowDemoWindow(&demo);
	}
}

void Editor::DrawDockSpace() {
	ImGuiWindowFlags dockSpaceWindowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking |
	                                        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
	                                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
	                                        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("DOCK_SPACE", nullptr, dockSpaceWindowFlags);

	ImGui::SetNextWindowBgAlpha(0.f);
	ImGui::DockSpace(ImGui::GetID("DOCK_SPACE_WINDOW"), {0.f, 0.f}, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
	ImGui::PopStyleVar(2);
}

}  // namespace JR