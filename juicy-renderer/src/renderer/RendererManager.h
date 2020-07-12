#pragma once

#include "JuicyRenderer.h"
#include "RenderCommand.h"

namespace JR {

class RendererManager {
public:
	RendererManager();
	~RendererManager();

	bool Init();

	void Render();

	void Submit(const RenderCommand& renderCommand);

private:
	void OnResize(int width, int height);
	void SetupImGuiStyle();
	void OnContentScale(glm::vec2 size);
	void UpdateScales();

	D3D11_VIEWPORT mViewport;

	JuicyRenderer mJuicyRenderer;

	std::vector<RenderCommand> mRenderCommands;

	struct FontData {
		float mScale;
		ImFont* mFont;
	};
	std::vector<FontData> mFonts;
	glm::vec2 mContentScale;

	MessageToken mResizeToken;
	MessageToken mContentScaleToken;
};

}  // namespace JR
