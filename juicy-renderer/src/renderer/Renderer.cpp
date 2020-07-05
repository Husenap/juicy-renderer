#include "Renderer.h"

#include "Window.h"

struct VertexData {
	float x, y, z, w;
	float u, v;
};

namespace JR {

Renderer::~Renderer() {
	if (mSwapChain) {
		mSwapChain->SetFullscreenState(FALSE, NULL);
	}
}

bool Renderer::Initialize() {
	if (!InitSwapChain()) {
		return false;
	}

	if (!InitResources()) {
		return false;
	}

	return true;
}

bool Renderer::InitSwapChain() {
	auto scd = DXGI_SWAP_CHAIN_DESC{.BufferDesc   = DXGI_MODE_DESC{.Format = DXGI_FORMAT_R8G8B8A8_UNORM},
	                                .SampleDesc   = DXGI_SAMPLE_DESC{.Count = 1},
	                                .BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	                                .BufferCount  = 1,
	                                .OutputWindow = mWindow.GetHandle(),
	                                .Windowed     = TRUE};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
	                                           D3D_DRIVER_TYPE_HARDWARE,
	                                           NULL,
	                                           NULL,
	                                           NULL,
	                                           NULL,
	                                           D3D11_SDK_VERSION,
	                                           &scd,
	                                           &mSwapChain,
	                                           &mDevice,
	                                           NULL,
	                                           &mContext);

	if (FAILED(hr)) {
		std::cerr << "Failed to create Swap Chain!" << std::endl;
		return false;
	}

	return true;
}

bool Renderer::InitResources() {
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr)) {
		std::cerr << "Failed to get Back Buffer!" << std::endl;
		return false;
	}

	hr = mDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &mRenderTarget);
	if (FAILED(hr)) {
		std::cerr << "Failed to create Render Target View" << std::endl;
		return false;
	}

	CD3D11_TEXTURE2D_DESC dbd(
	    DXGI_FORMAT_D32_FLOAT, mWindow.GetWidth(), mWindow.GetHeight(), 1, 1, D3D11_BIND_DEPTH_STENCIL);

	hr = mDevice->CreateTexture2D(&dbd, nullptr, &mDepthBuffer);
	if (FAILED(hr)) {
		std::cerr << "Failed to create Depth Buffer!" << std::endl;
		return false;
	}

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(mDepthBuffer.Get(), D3D11_DSV_DIMENSION_TEXTURE2D);

	hr = mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsvd, &mDepthStencil);
	if (FAILED(hr)) {
		std::cerr << "Failed to create Depth Stencil View" << std::endl;
		return false;
	}

	auto viewport = D3D11_VIEWPORT{
	    .TopLeftX = 0,
	    .TopLeftY = 0,
	    .Width    = static_cast<float>(mWindow.GetWidth()),
	    .Height   = static_cast<float>(mWindow.GetHeight()),
	};

	mContext->RSSetViewports(1, &viewport);

	if (!mShader.Load(mDevice, mContext, "assets/shaders/basic.hlsl")) {
		return false;
	}

	VertexData vertices[] = {{0.f, 0.5f, 1.f, 1.0f, 0.5f, 1.0f}, {0.45f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f}, {-0.45f, -0.5f, 1.0f, 1.0f, 0.f, 0.f}};

	CD3D11_BUFFER_DESC vbd(
	    sizeof(VertexData) * 3, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	hr = mDevice->CreateBuffer(&vbd, nullptr, &mVertexBuffer);
	if (FAILED(hr)) {
		std::cerr << "Failed to create Vertex Buffer" << std::endl;
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	mContext->Map(mVertexBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	std::memcpy(ms.pData, vertices, sizeof(vertices));
	mContext->Unmap(mVertexBuffer.Get(), NULL);

	return true;
}

void Renderer::Render() {
	ID3D11RenderTargetView* rtvs[] = {mRenderTarget.Get()};
	mContext->OMSetRenderTargets(1, rtvs, nullptr);

	float fBlue         = std::cosf(static_cast<float>(glfwGetTime())) * 0.5f + 0.5f;
	float clearColor[4] = {1.0f, 0.3725f, fBlue, 1.0f};  // PANTONE 812 C

	mContext->ClearRenderTargetView(mRenderTarget.Get(), clearColor);
	//mContext->ClearDepthStencilView(mDepthStencil.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	mShader.Bind(mContext);

	UINT stride = sizeof(VertexData);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffers[] = { mVertexBuffer.Get()};
	mContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mContext->Draw(3, 0);

	mSwapChain->Present(0, 0);
}

}  // namespace JR
