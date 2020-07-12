#include "Hierarchy.h"

#include "components/Components.h"
#include "editor/EditorUtil.h"

namespace JR::Widgets {

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

}  // namespace JR::Views