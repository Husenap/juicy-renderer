#pragma once

#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"
#include "BlendState.h"

namespace JR {

class Framework : public Module {
public:
	Framework() = default;
	~Framework();

	bool Initialize();

	bool InitSwapChain();
	bool InitResources();

	void Render();

	const ComPtr<ID3D11Device>& Device() const { return mDevice; }
	const ComPtr<ID3D11DeviceContext>& Context() const { return mContext; }

private:
	ComPtr<IDXGISwapChain> mSwapChain;
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mContext;

	ComPtr<ID3D11RenderTargetView> mRenderTarget;

	ComPtr<ID3D11Texture2D> mDepthBuffer;
	ComPtr<ID3D11DepthStencilView> mDepthStencil;

	Buffer mVertexBuffer;
	Shader mShader;

	Texture mTextureColor;
	Texture mTextureBack;

	BlendState mAlphaBlendState;
	BlendState mPremultipliedBlendState;
};

}  // namespace JR