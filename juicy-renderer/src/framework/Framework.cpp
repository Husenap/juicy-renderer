#include "Framework.h"

#include "Window.h"

namespace JR {

Framework::~Framework() {
	if (mSwapChain) {
		mSwapChain->SetFullscreenState(FALSE, NULL);
	}
}

bool Framework::Initialize() {
	if (!InitSwapChain()) {
		return false;
	}

	if (!InitResources()) {
		return false;
	}

	mResizeToken = MM::Get<Window>().Subscribe<EventResize>(
	    [&](const EventResize& message) { ResizeBackbuffer(message.width, message.height); });

	return true;
}

bool Framework::InitSwapChain() {
	IDXGIAdapter* adapter = FindBestAdapter();
	if (!adapter) {
		LOG_FATAL("Failed to find an adapter!");
		return false;
	}

	auto scd = DXGI_SWAP_CHAIN_DESC{.BufferDesc   = DXGI_MODE_DESC{.Format = DXGI_FORMAT_R8G8B8A8_UNORM},
	                                .SampleDesc   = DXGI_SAMPLE_DESC{.Count = 1},
	                                .BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	                                .BufferCount  = 1,
	                                .OutputWindow = MM::Get<Window>().GetHandle(),
	                                .Windowed     = TRUE};

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1,
	                                     D3D_FEATURE_LEVEL_11_0,
	                                     D3D_FEATURE_LEVEL_10_1,
	                                     D3D_FEATURE_LEVEL_10_0,
	                                     D3D_FEATURE_LEVEL_9_3,
	                                     D3D_FEATURE_LEVEL_9_1};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(adapter,
	                                           D3D_DRIVER_TYPE_UNKNOWN,
	                                           NULL,
	                                           creationFlags,
	                                           featureLevels,
	                                           ARRAYSIZE(featureLevels),
	                                           D3D11_SDK_VERSION,
	                                           &scd,
	                                           &mSwapChain,
	                                           &mDevice,
	                                           NULL,
	                                           &mContext);

	if (FAILED(hr)) {
		LOG_FATAL("Failed to create Swap Chain!");
		return false;
	}

	return true;
}

bool Framework::InitResources() {
	CreateTargets(MM::Get<Window>().GetWidth(), MM::Get<Window>().GetHeight());

	if (!mRendererManager.Init()) {
		return false;
	}

	return true;
}

void Framework::Render() {
	mRendererManager.Render();
}

void Framework::BeginFrame() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Framework::EndFrame() {
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	mSwapChain->Present(1, 0);
}

IDXGIAdapter* Framework::FindBestAdapter() {
	ComPtr<IDXGIFactory> factory;

	if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&factory)))) {
		LOG_FATAL("Failed to create DXGI Factory");
		return nullptr;
	}

	IDXGIAdapter* adapter;
	std::vector<IDXGIAdapter*> adapters;
	for (auto i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(adapter);
	}

	if (adapters.empty()) {
		LOG_FATAL("No adapters found");
		return nullptr;
	}

	IDXGIAdapter* bestAdapter = nullptr;
	for (auto i = 0; i < adapters.size(); ++i) {
		DXGI_ADAPTER_DESC adapterDesc;
		adapters[i]->GetDesc(&adapterDesc);

		LOG_INFO(
		    "Found adapter: %ls VRAM: %uMB", adapterDesc.Description, adapterDesc.DedicatedVideoMemory / (1 << 20));

		if (bestAdapter) {
			DXGI_ADAPTER_DESC bestAdapterDesc;
			bestAdapter->GetDesc(&bestAdapterDesc);

			if (bestAdapterDesc.DedicatedVideoMemory < adapterDesc.DedicatedVideoMemory) {
				bestAdapter = adapters[i];
			}
		} else {
			bestAdapter = adapters[i];
		}
	}

	return bestAdapter;
}

bool Framework::CreateTargets(int width, int height) {
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr)) {
		LOG_ERROR("Failed to get Back Buffer from Swap Chain");
		return false;
	}

	hr = mDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &mBackBuffer);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create Render Target View");
		return false;
	}

	/*
	CD3D11_TEXTURE2D_DESC dbd(DXGI_FORMAT_D32_FLOAT, width, height, 1, 1, D3D11_BIND_DEPTH_STENCIL);

	hr = mDevice->CreateTexture2D(&dbd, nullptr, &mDepthBuffer);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create Depth Buffer");
		return false;
	}

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(mDepthBuffer.Get(), D3D11_DSV_DIMENSION_TEXTURE2D);

	hr = mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsvd, &mDepthStencil);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create Depth Stencil View");
		return false;
	}

	CD3D11_DEPTH_STENCIL_DESC dstd(TRUE,
	                               D3D11_DEPTH_WRITE_MASK_ALL,
	                               D3D11_COMPARISON_LESS_EQUAL,
	                               FALSE,
	                               0xFF,
	                               0xFF,
	                               D3D11_STENCIL_OP_KEEP,
	                               D3D11_STENCIL_OP_INCR,
	                               D3D11_STENCIL_OP_KEEP,
	                               D3D11_COMPARISON_ALWAYS,
	                               D3D11_STENCIL_OP_ZERO,
	                               D3D11_STENCIL_OP_ZERO,
	                               D3D11_STENCIL_OP_ZERO,
	                               D3D11_COMPARISON_NEVER);
	hr = mDevice->CreateDepthStencilState(&dstd, &mDepthStencilState);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create Depth Stencil State");
		return false;
	}
	*/

	return true;
}

void Framework::ResizeBackbuffer(int width, int height) {
	if (width == 0 || height == 0) {
		return;
	}

	mContext->OMSetRenderTargets(0, nullptr, nullptr);

	mBackBuffer.Reset();
	//mDepthBuffer.Reset();
	//mDepthStencil.Reset();

	HRESULT hr = mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to Resize Buffers");
		return;
	}

	CreateTargets(width, height);
}

}  // namespace JR
