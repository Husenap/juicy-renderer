#pragma once

#include "BlendState.h"
#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"

#include "renderer/RendererManager.h"

namespace JR {

class Framework : public Module {
public:
	Framework() = default;
	~Framework();

	bool Initialize();

	bool InitSwapChain();

	bool InitResources();

	void Render();

	void BeginFrame();
	void EndFrame();

	const ComPtr<ID3D11Device>& Device() const { return mDevice; }
	const ComPtr<ID3D11DeviceContext>& Context() const { return mContext; }

private:
	bool CreateTargets(int width, int height);
	void ResizeBackbuffer(int width, int height);

	ComPtr<IDXGISwapChain> mSwapChain;
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mContext;

	ComPtr<ID3D11RenderTargetView> mRenderTarget;
	ComPtr<ID3D11Texture2D> mDepthBuffer;
	ComPtr<ID3D11DepthStencilView> mDepthStencil;
	ComPtr<ID3D11DepthStencilState> mDepthStencilState;

	BlendState mAlphaBlendState;
	BlendState mPremultipliedBlendState;

	MessageToken mResizeToken;

	RendererManager mRendererManager;
};

}  // namespace JR