#include "Viewport.h"

#include "framework/Framework.h"

namespace JR::Widgets {

void Viewport::Draw() {
	const Texture& renderTarget = MM::Get<Framework>().Renderer().GetRenderTarget();

	ImVec2 regionMin  = ImGui::GetWindowContentRegionMin();
	ImVec2 regionMax  = ImGui::GetWindowContentRegionMax();
	ImVec2 offset     = regionMin;
	ImVec2 regionSize = ImVec2(regionMax.x - regionMin.x, regionMax.y - regionMin.y);
	ImVec2 imageSize  = ImVec2(renderTarget.GetPixelSize().x, renderTarget.GetPixelSize().y);

	float regionRatio = regionSize.x / regionSize.y;
	float imageRatio  = imageSize.x / imageSize.y;

	if (regionRatio > imageRatio) {
		imageSize.x *= regionSize.y / imageSize.y;
		imageSize.y = regionSize.y;
	} else {
		imageSize.y *= regionSize.x / imageSize.x;
		imageSize.x = regionSize.x;
	}

	ImGui::SetCursorPosX((regionSize.x - imageSize.x) * 0.5f + offset.x);
	ImGui::SetCursorPosY((regionSize.y - imageSize.y) * 0.5f + offset.y);

	ImGui::Image(renderTarget, imageSize);
}

}  // namespace JR::Widgets