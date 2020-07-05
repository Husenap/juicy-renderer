#pragma once

#include "Shader.h"

namespace JR {
class Window;
}

namespace JR {

class Renderer {
public:
	Renderer(Window& window)
	    : mWindow(window) {}
	~Renderer();

	bool Initialize();

	bool InitSwapChain();
	bool InitResources();

	void Render();

private:
	Window& mWindow;

	ComPtr<IDXGISwapChain> mSwapChain;
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mContext;

	ComPtr<ID3D11RenderTargetView> mRenderTarget;

	ComPtr<ID3D11Texture2D> mDepthBuffer;
	ComPtr<ID3D11DepthStencilView> mDepthStencil;

	ComPtr<ID3D11Buffer> mVertexBuffer;

	Shader mShader;
};

}  // namespace JR