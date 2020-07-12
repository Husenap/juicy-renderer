#include "Editor.h"

#include "components/Components.h"
#include "framework/Framework.h"
#include "framework/Window.h"

namespace JR {

using namespace Components;

Editor::Editor(entt::registry& ecs)
    : mECS(ecs) {
	mKeyPressToken = MM::Get<Window>().Subscribe<EventKeyPress>([&](const auto& e) {
		if (!mProjectManager.IsLoaded()) {
			return;
		}
		if (e.key == GLFW_KEY_F11) {
			mShowEditor = !mShowEditor;
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
	DrawInspector();
	DrawHierarchy();
	DrawHistory();
	DrawContentBrowser();

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

void Editor::DrawInspector() {
	if (ImGui::Begin("Inspector")) {
		if (mSelectedEntity) {
			for (auto& drawer : mComponentDrawers) {
				drawer(*mSelectedEntity);
			}
		}
	}
	ImGui::End();
}

void Editor::DrawHierarchy() {
	if (ImGui::Begin("Hierarchy")) {
		const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
		                                     ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

		auto view = mECS.view<Identification>();

		std::optional<entt::entity> clickedEntity;
		for (auto entity : view) {
			auto& identification = view.get<Identification>(entity);

			ImGuiTreeNodeFlags nodeFlags = baseFlags;
			if (mSelectedEntity && mSelectedEntity == entity) {
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			bool nodeOpen = ImGui::TreeNodeEx((void*)entity, nodeFlags, "%s", identification.name);

			if (ImGui::IsItemClicked()) {
				clickedEntity = entity;
			}

			if (nodeOpen) {
				ImGui::TreePop();
			}
		}

		if (clickedEntity) {
			mSelectedEntity = clickedEntity;
		}
	}
	ImGui::End();
}

void Editor::DrawHistory() {
	if (ImGui::Begin("History")) {
		const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth |
		                                     ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen |
		                                     ImGuiTreeNodeFlags_Bullet;

		MM::Get<TransactionManager>().EnumerateUndoStack([](const std::string& commitMessage, bool isActive) {
			ImGuiTreeNodeFlags nodeFlags = baseFlags;
			if (isActive) {
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}
			ImGui::TreeNodeEx(commitMessage.c_str(), nodeFlags);
		});
		MM::Get<TransactionManager>().EnumerateRedoStack([](const std::string& commitMessage) {
			ImGuiTreeNodeFlags nodeFlags = baseFlags;
			ImGui::TreeNodeEx(commitMessage.c_str(), nodeFlags);
		});
	}
	ImGui::End();
}

void Editor::DrawContentBrowser() {
	if (ImGui::Begin("Content Browser")) {
	}
	ImGui::End();
}

}  // namespace JR