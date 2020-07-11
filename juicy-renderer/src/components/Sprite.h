#pragma once

namespace JR::Components {

struct Sprite {
	glm::vec4 uv;
	glm::vec4 tint;
	float blendMode;
};

void View(Sprite& sprite) {
	if (ImGui::CollapsingHeader("Sprite")) {
		ImGui::DragFloat4("uv", &sprite.uv.x, 0.05f);
		ImGui::ColorEdit4("tint", &sprite.tint.x);
		ImGui::SliderFloat("Blend Mode", &sprite.blendMode, 0.0f, 1.0f, "Additive - %.3f - Alpha Blend");
	}
}

}  // namespace JR::Components