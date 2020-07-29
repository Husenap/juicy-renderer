#pragma once

#include "editor/ContentManager.h"
#include "editor/DiffUtil.h"
#include "framework/TextureManager.h"

namespace JR::Components {

struct Sprite {
	glm::vec4 uv    = {0.f, 0.f, 1.f, 1.f};
	glm::vec4 tint  = {1.f, 1.f, 1.f, 1.f};
	float blendMode = 1.f;
	StringId texture;
	StringId backTexture;

	[[nodiscard]] constexpr static const char* GetDisplayName() { return "Sprite"; }
	[[nodiscard]] constexpr static bool IsUserComponent() { return true; }
};

static void SetTextureId(Sprite& sprite, StringId& textureId, const char* label) {
	auto& contentManager = MM::Get<ContentManager>();
	std::string texturePath;
	auto path = contentManager.GetPath(textureId);
	if (path) {
		texturePath = contentManager.GetRelativePath(*path).generic_string();
	}

	auto& texture        = MM::Get<TextureManager>().GetTexture(textureId);
	auto& defaultTexture = MM::Get<TextureManager>().GetDefaultTexture();

	ImGui::BeginGroup();

	auto cursorPos     = ImGui::GetCursorPos();
	auto thumbnailSize = ImVec2(100.f, 100.f);
	ImGui::Image(defaultTexture, thumbnailSize);
	ImGui::SetCursorPos(cursorPos);
	ImGui::Image(texture, thumbnailSize);

	ImGui::PushID(&textureId);
	if (ImGui::InputText(label, &texturePath, ImGuiInputTextFlags_AutoSelectAll)) {
		auto newId = StringId::FromPath(texturePath);
		if (contentManager.GetPath(newId)) {
			textureId = newId;
		}
	}
	DiffUtil::HandleTransaction(sprite, label);
	ImGui::PopID();

	ImGui::EndGroup();

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
}

static void View(Sprite& sprite) {
	ImGui::DragFloat4("UV", &sprite.uv.x, 0.05f);
	DiffUtil::HandleTransaction(sprite, "Sprite UV");

	ImGui::ColorEdit4("Tint", &sprite.tint.x);
	DiffUtil::HandleTransaction(sprite, "Sprite Tint");

	ImGui::SliderFloat("Blend Mode", &sprite.blendMode, 0.0f, 1.0f, "Additive - %.2f - Alpha Blend");
	DiffUtil::HandleTransaction(sprite, "Sprite Blend Mode");

	ImGui::Separator();

	SetTextureId(sprite, sprite.texture, "Sprite Texture");

	ImGui::Separator();

	SetTextureId(sprite, sprite.backTexture, "Sprite Back Texture");
}

}  // namespace JR::Components