#include "Inspector.h"

#include "editor/EditorUtil.h"

namespace JR::Widgets {

void Inspector::Draw() {
	auto selectedEntity = EditorUtil::GetSelectedEntity();

	if (selectedEntity != entt::null) {
		ImGui::PushItemWidth(-170.f);
		for (auto& drawer : mComponentDrawers) {
			drawer(selectedEntity);
		}
		ImGui::PopItemWidth();

		ImGui::Separator();

		if (ImGui::BeginCombo("Add Component", "Choose Component...")) {
			for (auto& listedComponent : mComponents) {
				bool wasSelected = false;
				ImGui::Selectable(listedComponent.componentName.c_str(), &wasSelected);
				if (wasSelected) {
					mComponentAdders[listedComponent.componentId](selectedEntity);
				}
			}
			ImGui::EndCombo();
		}
	}
}

}  // namespace JR::Widgets
