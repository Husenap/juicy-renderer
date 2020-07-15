#pragma once

#include "editor/DiffUtil.h"

namespace JR::Components {

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

static void View(Transform& transform) {
	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::DragFloat3("Position", &transform.position.x, 0.05f);
		DiffUtil::HandleTransaction(transform, "Transform Position");

		ImGui::DragFloat3("Rotation", &transform.rotation.x, 0.05f);
		DiffUtil::HandleTransaction(transform, "Transform Rotation");

		ImGui::DragFloat3("Scale", &transform.scale.x, 0.05f);
		DiffUtil::HandleTransaction(transform, "Transform Scale");
	}
}

}  // namespace JR::Components