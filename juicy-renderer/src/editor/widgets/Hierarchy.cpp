#include "Hierarchy.h"

#include "components/Components.h"
#include "editor/EditorUtil.h"
#include "framework/Window.h"

namespace JR::Widgets {

Hierarchy::Hierarchy(ECS& ecs)
    : Widget("Hierarchy")
    , mECS(ecs) {
	mKeyPressToken = MM::Get<Window>().Subscribe<EventKeyPress>([&](const auto& e) {
		if (e.key == GLFW_KEY_N && e.mods == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT)) {
			CreateNewEntity();
		}
		if (e.key == GLFW_KEY_DELETE) {
			DeleteSelectedEntity();
		}
	});
}

void Hierarchy::Draw() {
	const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
	                                     ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	auto view = mECS.view<Components::Identification>();

	auto selectedEntity = EditorUtil::GetSelectedEntity();

	for (auto entity : view) {
		auto& identification = view.get<Components::Identification>(entity);

		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		if (selectedEntity == entity) {
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		bool nodeOpen = ImGui::TreeNodeEx((void*)entity, nodeFlags, "%s", identification.name);

		if (ImGui::IsItemClicked()) {
			EditorUtil::SelectEntity(entity);
		}

		if (nodeOpen) {
			ImGui::TreePop();
		}
	}
}

void Hierarchy::DrawContextMenu() {
	if (ImGui::BeginPopupContextItem("HIERARCHY_CONTEXT_MENU")) {
		if (ImGui::Selectable("Create Entity")) {
			CreateNewEntity();
		}
		if (ImGui::Selectable("Delete Entity")) {
			DeleteSelectedEntity();
		}

		ImGui::EndPopup();
	}
}

void Hierarchy::DeleteSelectedEntity() {
	auto selectedEntity = EditorUtil::GetSelectedEntity();
	if (selectedEntity != entt::null) {
		MM::Get<TransactionManager>().RemoveEntity(
		    selectedEntity, "Removed Entity", CreateEntitySnapshot(selectedEntity));
	}
}

void Hierarchy::CreateNewEntity() {
	auto entity = mECS.create();
	mECS.emplace<Components::Identification>(entity);
	mECS.emplace<Components::Transform>(entity);

	MM::Get<TransactionManager>().AddEntity(entity, "Added Entity", CreateEntitySnapshot(entity));
}

}  // namespace JR::Widgets