#include "ContentBrowser.h"

#include "editor/ContentManager.h"
#include "framework/Texture.h"
#include "framework/TextureManager.h"

namespace JR::Widgets {

constexpr float MIN_ICON_SCALE = 16.f;
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

	std::vector<std::filesystem::path> files;

	for (auto p : std::filesystem::directory_iterator(*mSelectedDirectory)) {
		auto path = p.path();
		if (std::filesystem::is_directory(path)) {
			continue;
		}

		auto relativePath = MM::Get<ContentManager>().GetRelativePath(path);

		files.push_back(relativePath);
	}

	auto& style          = ImGui::GetStyle();
	float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	bool useListView     = mZoom < 50.f;

	const ImGuiWindowFlags windowFlags =
	    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	for (auto i = 0; i < files.size(); ++i) {
		auto& filepath = files[i];
		auto textureId = StringId::FromPath(filepath);
		auto& texture  = MM::Get<TextureManager>().GetTexture(textureId);

		auto size =
		    ImVec2(mZoom + style.FramePadding.x, mZoom + ImGui::GetTextLineHeightWithSpacing() + style.FramePadding.y);
		if (useListView) {
			size = ImVec2(0.f, mZoom + style.FramePadding.y);
		}

		if (ImGui::BeginChild(filepath.filename().string().c_str(), size, true, windowFlags)) {
			ImGui::BeginGroup();
			ImGui::Image(texture, {mZoom, mZoom});
			if (useListView) {
				ImGui::SameLine();
			}
			ImGui::Text("%s", filepath.filename().string().c_str());
			ImGui::EndGroup();

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				ImGui::SetDragDropPayload("TEXTURE_ID", &textureId, sizeof(textureId));
				ImGui::Image(texture, {50.f, 50.f});
				ImGui::SameLine();
				ImGui::Text("%s", filepath.filename().string().c_str());
				ImGui::EndDragDropSource();
			}
		}
		ImGui::EndChild();

		if (!useListView) {
			float lastButtonX = ImGui::GetItemRectMax().x;
			float nextButtonX = lastButtonX + style.ItemSpacing.x + mZoom;
			if (i + 1 < files.size() && nextButtonX < windowVisibleX) {
				ImGui::SameLine();
			}
		}
	}
}

}  // namespace JR::Widgets