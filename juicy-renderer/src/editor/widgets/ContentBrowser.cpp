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

void ContentBrowser::DrawTreeRecursive(std::filesystem::path currentPath) {
	const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
	                                     ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	ImGuiTreeNodeFlags nodeFlags = baseFlags;

	if (mSelectedDirectory && *mSelectedDirectory == currentPath) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	bool nodeOpen = ImGui::TreeNodeEx(currentPath.filename().string().c_str(), nodeFlags);

	if (ImGui::IsItemClicked()) {
		mSelectedDirectory = currentPath;
	}

	if (nodeOpen) {
		for (auto p : std::filesystem::directory_iterator(currentPath)) {
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
	    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysUseWindowPadding;

	std::optional<StringId> hoveredFile;
	for (auto i = 0; i < files.size(); ++i) {
		auto& filepath = files[i];
		auto fileId = StringId::FromPath(filepath);
		auto& texture  = MM::Get<TextureManager>().GetTexture(fileId);
		auto& defaultTexture  = MM::Get<TextureManager>().GetDefaultTexture();

		auto size =
		    ImVec2(mZoom + style.WindowPadding.x*2.0f, mZoom + ImGui::GetTextLineHeightWithSpacing() + style.WindowPadding.y*2.0f);
		if (useListView) {
			size = ImVec2(0.f, mZoom + style.WindowPadding.y*2.0f);
		}

		if (mHoveredFile && *mHoveredFile == fileId) {
			ImGui::PushStyleColor(ImGuiCol_ChildBg, {0.35f, 0.35f, 0.35f, 1.00f});
		}
		if (ImGui::BeginChild(filepath.filename().string().c_str(), size, false, windowFlags)) {
			ImGui::BeginGroup();
			auto cursorPos = ImGui::GetCursorPos();
			ImGui::Image(defaultTexture, {mZoom, mZoom});
			ImGui::SetCursorPos(cursorPos);
			ImGui::Image(texture, {mZoom, mZoom});
			if (useListView) {
				ImGui::SameLine();
			}
			ImGui::Text("%s", filepath.filename().string().c_str());
			ImGui::EndGroup();

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				ImGui::SetDragDropPayload("TEXTURE_ID", &fileId, sizeof(fileId));
				cursorPos = ImGui::GetCursorPos();
				auto thumbnailSize = ImVec2(50.f, 50.f);
				ImGui::Image(defaultTexture, thumbnailSize);
				ImGui::SetCursorPos(cursorPos);
				ImGui::Image(texture, thumbnailSize);
				ImGui::SameLine();
				ImGui::Text("%s", filepath.filename().string().c_str());
				ImGui::EndDragDropSource();
			}
		}
		ImGui::EndChild();
		if (mHoveredFile && *mHoveredFile == fileId) {
			ImGui::PopStyleColor();
		}
		if (ImGui::IsItemHovered()) {
			hoveredFile = fileId;
		}

		if (!useListView) {
			float lastButtonX = ImGui::GetItemRectMax().x;
			float nextButtonX = lastButtonX + style.ItemSpacing.x + mZoom;
			if (i + 1 < files.size() && nextButtonX < windowVisibleX) {
				ImGui::SameLine();
			}
		}
	}

	mHoveredFile = hoveredFile;
}

}  // namespace JR::Widgets