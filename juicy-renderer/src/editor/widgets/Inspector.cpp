#include "Inspector.h"

#include "editor/EditorUtil.h"

namespace JR::Widgets {

void Inspector::Draw() {
	auto selectedEntity = EditorUtil::GetSelectedEntity();

	if (selectedEntity) {
		for (auto& drawer : mComponentDrawers) {
			drawer(*selectedEntity);
		}
	}
}

}  // namespace JR::Views
