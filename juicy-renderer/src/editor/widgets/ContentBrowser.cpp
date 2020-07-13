#include "ContentBrowser.h"

#include "editor/ContentManager.h"
#include "framework/Texture.h"
#include "framework/TextureManager.h"

namespace JR::Widgets {

constexpr float MIN_ICON_SCALE = 10.f;
constexpr float MAX_ICON_SCALE = 250.f;

void ContentBrowser::Draw() {
	if (ImGui::BeginChild("CONTENT_BROWSER_BODY", {0, -ImGui::GetTextLineHeight() * 2.f})) {
		ImGui::Columns(2);

		if (ImGui::BeginChild("CONTENT_BROWSER_TREE")) {
			DrawTree();
		}
		ImGui::EndChild();

		ImGui::NextColumn();
		ImGui::Separator();

		if (ImGui::BeginChild("CONTENT_BROWSER_CONTENT")) {
			DrawContent();
		}
		ImGui::EndChild();

		ImGui::Columns(1);
	}
	ImGui::EndChild();

	ImGui::Separator();

	if (ImGui::BeginChild("CONTENT_BROWSER_FOOTER")) {
		ImGui::SetNextItemWidth(200.f);
		ImGui::SliderFloat("Icon Scale", &mZoom, MIN_ICON_SCALE, MAX_ICON_SCALE, "");
	}
	ImGui::EndChild();
}

void ContentBrowser::DrawTree() {
	auto contentPath = MM::Get<ContentManager>().GetContentPath();

	DrawTreeRecursive(contentPath);
}

void ContentBrowser::DrawTreeRecursive(std::filesystem::path path) {
	const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
	                                     ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	ImGuiTreeNodeFlags nodeFlags = baseFlags;

	if (mSelectedDirectory && *mSelectedDirectory == path) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	bool nodeOpen = ImGui::TreeNodeEx(path.filename().string().c_str(), nodeFlags);

	if (ImGui::IsItemClicked()) {
		mSelectedDirectory = path;
	}

	if (nodeOpen) {
		for (auto p : std::filesystem::directory_iterator(path)) {
			auto path = p.path();
			if (!std::filesystem::is_directory(path)) {
				continue;
			}

			DrawTreeRecursive(path);
		}
		ImGui::TreePop();
	}
}

void ContentBrowser::DrawContent() {
	if (!mSelectedDirectory) {
		return;
	}

	auto& style          = ImGui::GetStyle();
	const int numButtons = 100;
	float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

	std::vector<std::filesystem::path> files;

	for (auto p : std::filesystem::directory_iterator(*mSelectedDirectory)) {
		auto path = p.path();
		if (std::filesystem::is_directory(path)) {
			continue;
		}

		auto relativePath = MM::Get<ContentManager>().GetRelativePath(path);

		files.push_back(relativePath);
	}

	for (auto i = 0; i < files.size(); ++i) {
		auto textureId = StringId::FromPath(files[i]);
		auto& texture  = MM::Get<TextureManager>().GetTexture(textureId);

		if (ImGui::ImageButton(texture, {mZoom, mZoom})) {
			LOG_DEBUG("PRESSED BUTTON!");
		}
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("TEXTURE_ID", &textureId, sizeof(textureId));
			ImGui::Image(texture, {50.f, 50.f});
			ImGui::EndDragDropSource();
		}

		float lastButtonX = ImGui::GetItemRectMax().x;
		float nextButtonX = lastButtonX + style.ItemSpacing.x + mZoom;
		if (i + 1 < files.size() && nextButtonX < windowVisibleX) {
			ImGui::SameLine();
		}
	}
}

}  // namespace JR::Widgets