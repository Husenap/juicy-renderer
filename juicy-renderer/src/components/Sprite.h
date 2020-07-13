#pragma once

#include "editor/ContentManager.h"
#include "editor/DiffUtil.h"

namespace JR::Components {

struct Sprite {
	glm::vec4 uv;
	glm::vec4 tint;
	float blendMode;
	StringId texture;
	StringId backTexture;
};

static void SetTextureId(Sprite& sprite, StringId& textureId, const char* label) {
	auto& contentManager = MM::Get<ContentManager>();
	std::string texturePath;
	auto path = contentManager.GetPath(textureId);
	if (path) {
		texturePath = contentManager.GetRelativePath(*path).generic_string();
	}

	ImGui::PushID(&textureId);
	if (ImGui::InputText(label, &texturePath, ImGuiInputTextFlags_AutoSelectAll)) {
		auto newId = StringId::FromPath(texturePath);
		if (contentManager.GetPath(newId)) {
			textureId = newId;
		}
	}
	DiffUtil::HandleTransaction(sprite, label);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ID")) {
			assert(payload->DataSize == sizeof(StringId));

			StringId newId = *(const StringId*)payload->Data;
			if (contentManager.GetPath(newId)) {
				DiffUtil::Snapshot(sprite);
				textureId = newId;
				DiffUtil::CommitChanges(sprite, label);
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::PopID();
}

static void View(Sprite& sprite) {
	if (ImGui::CollapsingHeader("Sprite")) {
		ImGui::DragFloat4("UV", &sprite.uv.x, 0.05f);
		DiffUtil::HandleTransaction(sprite, "Sprite UV");

		ImGui::ColorEdit4("Tint", &sprite.tint.x);
		DiffUtil::HandleTransaction(sprite, "Sprite Tint");

		ImGui::SliderFloat("Blend Mode", &sprite.blendMode, 0.0f, 1.0f, "Additive - %.3f - Alpha Blend");
		DiffUtil::HandleTransaction(sprite, "Sprite Blend Mode");

		SetTextureId(sprite, sprite.texture, "Texture");

		SetTextureId(sprite, sprite.backTexture, "Back Texture");
	}
}

}  // namespace JR::Components