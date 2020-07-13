#pragma once

#include "JuicyRenderer.h"
#include "RenderCommand.h"

namespace JR {

class RenderManager {
public:
	RenderManager();
	~RenderManager();

	bool Init();

	void Render();

	void Submit(const RenderCommand& renderCommand);

	const Texture& GetRenderTarget() const { return mRenderTarget; }

private:
	void CopyRenderTargetToBackBuffer();

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

	SamplerState mSamplerState;
	Texture mRenderTarget;
	Shader mCopyShader;

	struct ScreenTriangleVertex {
		glm::vec2 pos;
		glm::vec2 uv;
	};
	Buffer mScreenTriangle;
};

}  // namespace JR
