#pragma once

namespace JR::Components {

enum LightDirection : int { Front, Back };
constexpr std::array<const char*, 2> LightDirectionStrings{"Front", "Back"};

struct Light {
	glm::vec3 color          = {1.f, 1.f, 1.f};
	float intensity          = 1.f;
	float size               = 1.f;
	LightDirection direction = LightDirection::Front;
	float flickerSpeed       = 0.f;
	float flickerIntensity   = 0.f;

	[[nodiscard]] constexpr static const char* GetDisplayName() { return "Light"; }
	[[nodiscard]] constexpr static bool IsUserComponent() { return true; }
};

static void View(Light& light) {
	ImGui::ColorEdit3("Color", &light.color.x);
	DiffUtil::HandleTransaction(light, "Light Color");

	ImGui::DragFloat("Intensity", &light.intensity, 0.1f, 0.f, 50.f);
	DiffUtil::HandleTransaction(light, "Light Intensity");

	ImGui::DragFloat("Size", &light.size, 0.1f, 0.f, 100.f);
	DiffUtil::HandleTransaction(light, "Light Size");

	LightDirection direction = light.direction;
	ImGui::Combo(
	    "Direction", (int*)(&direction), LightDirectionStrings.data(), static_cast<int>(LightDirectionStrings.size()));
	if (direction != light.direction) {
		DiffUtil::Snapshot(light);
		light.direction = direction;
		DiffUtil::CommitChanges(light, "Light Direction");
	}

	ImGui::DragFloat("Flicker Speed", &light.flickerSpeed, 0.1f, 0.f, 100.f);
	DiffUtil::HandleTransaction(light, "Light Flicker Speed");

	ImGui::DragFloat("Flicker Intensity", &light.flickerIntensity, 0.1f, 0.f, 1.f);
	DiffUtil::HandleTransaction(light, "Light Flicker Intensity");
}

}  // namespace JR::Components