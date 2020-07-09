#pragma once

#include "JuicyRenderer.h"

namespace JR {

class RendererManager {
public:
	RendererManager();
	~RendererManager();

	bool Init();

	void Render();

	void SetupImGuiStyle();

private:
	void OnResize(int width, int height);

	D3D11_VIEWPORT mViewport;

	JuicyRenderer mJuicyRenderer;

	MessageToken mResizeToken;
};

}  // namespace JR
