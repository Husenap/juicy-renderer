#include "Hierarchy.h"

#include "components/Components.h"
#include "editor/EditorUtil.h"
#include "framework/Window.h"

namespace JR::Widgets {

static auto CreateEntitySnapshot(entt::entity entity) {

}

Hierarchy::Hierarchy(ECS& ecs)
    : Widget("Hierarchy")
    , mECS(ecs) {
	mKeyPressToken = MM::Get<Window>().Subscribe<EventKeyPress>([&](const auto& e) {
		if (e.key == GLFW_KEY_N && e.mods == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT)) {
			auto entity = mECS.create();
			mECS.emplace<Components::Identification>(entity);
			mECS.emplace<Components::Transform>(entity);

			MM::Get<TransactionManager>().AddEntity(entity, "Added Entity", CreateEntitySnapshot(entity));
		}
		if (e.key == GLFW_KEY_DELETE) {
			auto selectedEntity = EditorUtil::GetSelectedEntity();
			if (selectedEntity) {
				MM::Get<TransactionManager>().RemoveEntity(*selectedEntity, "Removed Entity", CreateEntitySnapshot(*selectedEntity));
			}
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
		if (selectedEntity && selectedEntity == entity) {
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

}  // namespace JR::Widgets