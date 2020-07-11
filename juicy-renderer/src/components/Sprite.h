#pragma once

namespace JR::Components {

struct Sprite {
	glm::vec4 uv;
	glm::vec4 tint;
	float blendMode;
};

void View(Sprite& sprite) {
	if (ImGui::CollapsingHeader("Sprite")) {
		ImGui::DragFloat4("UV", &sprite.uv.x, 0.05f);
		DiffUtil::HandleTransaction(sprite, "Sprite UV");

		ImGui::ColorEdit4("Tint", &sprite.tint.x);
		DiffUtil::HandleTransaction(sprite, "Sprite Tint");

		ImGui::SliderFloat("Blend Mode", &sprite.blendMode, 0.0f, 1.0f, "Additive - %.3f - Alpha Blend");
		DiffUtil::HandleTransaction(sprite, "Sprite Blend Mode");
	}
}

}  // namespace JR::Components