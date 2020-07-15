#include "Inspector.h"

#include "editor/EditorUtil.h"

namespace JR::Widgets {

void Inspector::Draw() {
	auto selectedEntity = EditorUtil::GetSelectedEntity();

	if (selectedEntity) {
		ImGui::PushItemWidth(-170.f);
		for (auto& drawer : mComponentDrawers) {
			drawer(*selectedEntity);
		}
		ImGui::PopItemWidth();
	}
}

}  // namespace JR::Widgets
