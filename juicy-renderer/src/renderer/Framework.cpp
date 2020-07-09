#include "Framework.h"

#include "Window.h"


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

	SetupImGuiStyle();

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


	mVertices = {
		{{-1.1f, 0.0f, 1.0f, 1.0f},{0.0, 0.0, 0.5, 0.5} },
		{{1.1f, 0.0f, 1.0f, 1.0f}, {0.5, 0.5, 1.0, 1.0} },
		{{7.5f, 0.0f, 7.5f, 1.0f}, {0.0, 0.5, 0.5, 1.0} },
	};
	if (!mVertexBuffer.Create(CD3D11_BUFFER_DESC(sizeof(VertexData) * 512,
	                                             D3D11_BIND_VERTEX_BUFFER,
	                                             D3D11_USAGE_DYNAMIC,
	                                             D3D11_CPU_ACCESS_WRITE))) {
		LOG_ERROR("Failed to create Vertex Buffer!");
		return false;
	}
	mVertexBuffer.SetData(mVertices);

	mConstantBuffer
	    .Create(CD3D11_BUFFER_DESC(sizeof(ConstantBufferData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE));

	mAlphaBlendState.Create(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_ZERO);
	mPremultipliedBlendState.Create(
	    D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA);

	return true;
}

void Framework::Render() {
	float t = (float)glfwGetTime();
	std::vector<VertexData> vertices = mVertices;
	std::sort(vertices.begin(), vertices.end(), [](auto a, auto b){return a.position.z > b.position.z;});
	mVertexBuffer.SetData(vertices);

	mContext->OMSetRenderTargets(1, mRenderTarget.GetAddressOf(), mDepthStencil.Get());

	glm::vec4 clearColor(0.3725f, 0.625f, 1.0f, 1.0f);
	clearColor = glm::vec4(0.0, 0.0, 0.0, 1.0);

	mContext->ClearRenderTargetView(mRenderTarget.Get(), &clearColor.r);
	mContext->ClearDepthStencilView(mDepthStencil.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	mContext ->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	mShader.Bind();
	mTextureColor.Bind(0);
	mTextureBack.Bind(1);

	mPremultipliedBlendState.Bind();

	mConstantBufferData.ProjectionMatrix = glm::perspectiveFovLH(glm::radians(80.f), mViewport.Width, mViewport.Height, 0.01f, 100.f);
	mConstantBuffer.SetData(mConstantBufferData);
	mConstantBuffer.Bind(0);

	mVertexBuffer.Bind(sizeof(VertexData), 0);

	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	mContext->Draw(vertices.size(), 0);

	mTextureColor.Unbind(0);
	mTextureBack.Unbind(0);
	mShader.Unbind();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DoGUI();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	mSwapChain->Present(1, 0);
}

void Framework::DoGUI() {
	ImGuiWindowFlags dockSpaceWindowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking |
	                                        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
	                                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
	                                        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("DOCK_SPACE", nullptr, dockSpaceWindowFlags);

	ImGui::SetNextWindowBgAlpha(0.f);
	ImGui::DockSpace(ImGui::GetID("DOCK_SPACE_WINDOW"), {0.f, 0.f}, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
	ImGui::PopStyleVar(2);

	static bool potato = true;
	if (potato) {
		ImGui::ShowDemoWindow(&potato);
	}

	if (ImGui::Begin("VERTICES")) {
		for (auto& vertex : mVertices) {
			ImGui::PushID(ImGui::GetID(&vertex));
			ImGui::DragFloat3("position", &vertex.position.x, 0.05f);
			ImGui::DragFloat4("uv", &vertex.uv.x, 0.05f);
			ImGui::ColorEdit4("tint", &vertex.tint.x);
			ImGui::SliderFloat("Blend Mode", &vertex.blendMode, 0.0f, 1.0f, "Additive - %.3f - Alpha Blend");
			ImGui::Separator();
			ImGui::PopID();
		}
	}
	ImGui::End();
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
	hr = mDevice->CreateDepthStencilState(&dstd,  &mDepthStencilState);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create Depth Stencil State");
		return false;
	}

	mViewport = D3D11_VIEWPORT{
	    .TopLeftX = 0,
	    .TopLeftY = 0,
	    .Width    = static_cast<float>(width),
	    .Height   = static_cast<float>(height),
	};

	mContext->RSSetViewports(1, &mViewport);

	mConstantBufferData.Resolution = glm::vec4(
		mViewport.Width,
		mViewport.Height,
		mViewport.Width/mViewport.Height,
		mViewport.Height/mViewport.Width
	);

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

void Framework::SetupImGuiStyle() {
	ImGuiStyle& style                      = ImGui::GetStyle();
	ImVec4* colors                         = style.Colors;
	colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]          = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_WindowBg]              = ImVec4(0.25f, 0.25f, 0.25f, 0.75f);
	colors[ImGuiCol_ChildBg]               = ImVec4(0.25f, 0.25f, 0.25f, 0.00f);
	colors[ImGuiCol_PopupBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Border]                = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
	colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_FrameBg]               = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive]         = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	colors[ImGuiCol_TitleBg]               = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive]         = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	colors[ImGuiCol_MenuBarBg]             = ImVec4(0.33f, 0.33f, 0.33f, 0.75f);
	colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_CheckMark]             = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrab]            = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button]                = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	colors[ImGuiCol_ButtonHovered]         = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	colors[ImGuiCol_ButtonActive]          = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_Header]                = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_HeaderHovered]         = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_HeaderActive]          = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	colors[ImGuiCol_Separator]             = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
	colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.70f, 0.67f, 0.60f, 0.29f);
	colors[ImGuiCol_SeparatorActive]       = ImVec4(0.70f, 0.67f, 0.60f, 0.67f);
	colors[ImGuiCol_ResizeGrip]            = ImVec4(0.99f, 0.78f, 0.61f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.78f, 0.62f, 0.48f, 1.00f);
	colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.86f, 0.68f, 0.53f, 1.00f);
	colors[ImGuiCol_Tab]                   = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabHovered]            = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_TabActive]             = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_TabUnfocused]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_DockingPreview]        = ImVec4(1.00f, 0.37f, 0.64f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.75f, 0.28f, 0.47f, 1.00f);
	colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.72f, 0.30f, 1.00f);
	colors[ImGuiCol_PlotHistogram]         = ImVec4(0.68f, 0.84f, 0.51f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.48f, 0.59f, 0.42f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	colors[ImGuiCol_DragDropTarget]        = ImVec4(0.51f, 0.83f, 0.98f, 1.00f);
	colors[ImGuiCol_NavHighlight]          = ImVec4(0.38f, 0.62f, 0.73f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	style.PopupRounding = 3.f;

	style.WindowPadding    = ImVec2(4.f, 4.f);
	style.FramePadding     = ImVec2(6.f, 4.f);
	style.ItemSpacing      = ImVec2(3.f, 3.f);
	style.ItemInnerSpacing = ImVec2(3.f, 3.f);

	style.ScrollbarSize = 18.f;

	style.WindowBorderSize = 1.f;
	style.ChildBorderSize  = 1.f;
	style.PopupBorderSize  = 1.f;
	style.FrameBorderSize  = 0.f;

	style.WindowRounding    = 2.f;
	style.ChildRounding     = 2.f;
	style.FrameRounding     = 2.f;
	style.ScrollbarRounding = 2.f;
	style.GrabRounding      = 3.f;

	style.TabBorderSize = 0.f;
	style.TabRounding   = 3.f;
}

}  // namespace JR
