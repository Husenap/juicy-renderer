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

	MessageToken mResizeToken;
	D3D11_VIEWPORT mViewport;

	JuicyRenderer mJuicyRenderer;

	std::vector<RenderCommand> mRenderCommands;
};

}  // namespace JR
