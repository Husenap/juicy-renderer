#include "Framework.h"

#include "Window.h"

struct VertexData {
	float x, y, z, w;
};

namespace JR {

Framework::~Framework() {
	ImGui_ImplDX11_Shutdown();

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

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
	                                           D3D_DRIVER_TYPE_HARDWARE,
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplDX11_Init(mDevice.Get(), mContext.Get());
	ImGui_ImplWin32_Init(MM::Get<Window>().GetHandle());

	return true;
}

bool Framework::InitResources() {
	CreateTargets(MM::Get<Window>().GetWidth(), MM::Get<Window>().GetHeight());

	if (!mShader.Load(Shader::Vertex | Shader::Geometry | Shader::Pixel, "assets/shaders/basic.hlsl")) {
		return false;
	}

	if (!mTextureColor.CreateFromFile("assets/textures/groundprops.png")) {
		return false;
	}
	if (!mTextureBack.CreateFromFile("assets/textures/groundprops_back.png")) {
		return false;
	}

	std::vector<VertexData> vertices = {{0.0f, 0.0f, 1.0f, 1.0f}};
	if (!mVertexBuffer.Create(CD3D11_BUFFER_DESC(sizeof(VertexData) * vertices.size(),
	                                             D3D11_BIND_VERTEX_BUFFER,
	                                             D3D11_USAGE_DYNAMIC,
	                                             D3D11_CPU_ACCESS_WRITE))) {
		LOG_ERROR("Failed to create Vertex Buffer!");
		return false;
	}
	mVertexBuffer.SetData(vertices);

	mAlphaBlendState.Create(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_ZERO);
	mPremultipliedBlendState.Create(
	    D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA);

	return true;
}

void Framework::Render() {
	mContext->OMSetRenderTargets(1, mRenderTarget.GetAddressOf(), mDepthStencil.Get());

	glm::vec4 clearColor(1.0f, 0.3725f, 0.625f, 1.0f);

	mContext->ClearRenderTargetView(mRenderTarget.Get(), &clearColor.r);
	mContext->ClearDepthStencilView(mDepthStencil.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	mShader.Bind();
	mTextureColor.Bind(0);
	mTextureBack.Bind(1);

	mPremultipliedBlendState.Bind();

	UINT stride = sizeof(VertexData);
	UINT offset = 0;
	mVertexBuffer.Bind(sizeof(VertexData), 0);

	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	mContext->Draw(1, 0);

	ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

	static bool potato = true;
	ImGui::ShowDemoWindow(&potato);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	mSwapChain->Present(0, 0);
}

bool Framework::CreateTargets(int width, int height) {
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr)) {
		LOG_ERROR("Failed to get Back Buffer from Swap Chain");
		return false;
	}

	hr = mDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &mRenderTarget);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create Render Target View");
		return false;
	}

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

	mViewport = D3D11_VIEWPORT{
	    .TopLeftX = 0,
	    .TopLeftY = 0,
	    .Width    = static_cast<float>(width),
	    .Height   = static_cast<float>(height),
	};

	mContext->RSSetViewports(1, &mViewport);

	return true;
}

void Framework::ResizeBackbuffer(int width, int height) {
	if (width == 0 || height == 0) {
		return;
	}

	mContext->OMSetRenderTargets(0, nullptr, nullptr);

	mRenderTarget.Reset();
	mDepthBuffer.Reset();
	mDepthStencil.Reset();

	HRESULT hr = mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to Resize Buffers");
		return;
	}

	CreateTargets(width, height);
}

}  // namespace JR
