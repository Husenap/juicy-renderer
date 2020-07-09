#pragma once

#include "BlendState.h"
#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"

namespace JR {

class Framework : public Module {
public:
	Framework() = default;
	~Framework();

	bool Initialize();

	bool InitSwapChain();

	bool InitResources();

	void Render();

	void DoGUI();

	const ComPtr<ID3D11Device>& Device() const { return mDevice; }
	const ComPtr<ID3D11DeviceContext>& Context() const { return mContext; }

private:
	bool CreateTargets(int width, int height);
	void ResizeBackbuffer(int width, int height);
	void SetupImGuiStyle();

	ComPtr<IDXGISwapChain> mSwapChain;
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mContext;

	ComPtr<ID3D11RenderTargetView> mRenderTarget;
	ComPtr<ID3D11Texture2D> mDepthBuffer;
	ComPtr<ID3D11DepthStencilView> mDepthStencil;
	ComPtr<ID3D11DepthStencilState> mDepthStencilState;
	D3D11_VIEWPORT mViewport;

	Shader mShader;
	struct VertexData {
		glm::vec4 position;
		glm::vec4 uv;
		glm::vec4 tint = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
		float blendMode = 1.0f;
	};
	std::vector<VertexData> mVertices;
	Buffer mVertexBuffer;

	struct ConstantBufferData {
		glm::mat4 ProjectionMatrix;
		glm::vec4 Resolution;
	} mConstantBufferData;
	Buffer mConstantBuffer;

	Texture mTextureColor;
	Texture mTextureBack;

	BlendState mAlphaBlendState;
	BlendState mPremultipliedBlendState;

	MessageToken mResizeToken;
};

}  // namespace JR