#pragma once

#include "editor/DiffUtil.h"

namespace JR::Components {

struct Transform {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
};

void View(Transform& transform) {
	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::DragFloat3("position", &transform.position.x, 0.05f);
		DiffUtil::HandleTransaction(transform, "Changed position");

		ImGui::DragFloat3("scale", &transform.scale.x, 0.05f);
		DiffUtil::HandleTransaction(transform, "Changed scale");

		ImGui::DragFloat3("rotation", &transform.rotation.x, 0.05f);
		DiffUtil::HandleTransaction(transform, "Changed rotation");
	}
}

}  // namespace JR::Components