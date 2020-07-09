#include "JuicyRenderer.h"

#include "framework/Framework.h"
#include "framework/Window.h"

namespace JR {
bool JuicyRenderer::Init() {
	if (!mShader.Load(Shader::Vertex | Shader::Geometry | Shader::Pixel, "assets/shaders/basic.hlsl")) {
		return false;
	}

	if (!mTextureColor.CreateFromFile("assets/textures/groundprops.png")) {
		return false;
	}
	if (!mTextureBack.CreateFromFile("assets/textures/groundprops_back.png")) {
		return false;
	}

	if (!mSpriteBuffer.Create(CD3D11_BUFFER_DESC(sizeof(mSprites[0]) * mSprites.max_size(),
	                                             D3D11_BIND_VERTEX_BUFFER,
	                                             D3D11_USAGE_DYNAMIC,
	                                             D3D11_CPU_ACCESS_WRITE))) {
		LOG_ERROR("Failed to create Vertex Buffer!");
		return false;
	}

	mConstantBuffer.Create(CD3D11_BUFFER_DESC(
	    sizeof(ConstantBufferData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE));

	if (!mPremultipliedBlendState.Create(
	        D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA)) {
		return false;
	}

	if (!mSamplerState.Create(SamplerState::Filter::Point, SamplerState::Address::Wrap)) {
		return false;
	}

	return true;
}
void JuicyRenderer::Render() {
	mPremultipliedBlendState.Bind();
	mSamplerState.Bind(0);
	mShader.Bind();
	mTextureColor.Bind(0);
	mTextureBack.Bind(1);

	UpdateConstantBuffer();
	mConstantBuffer.Bind(0);

	sizeof(RCSprite);

	std::size_t remaining = mSpriteRenderCommands.size();
	while (remaining > 0) {
		std::size_t batchSize = std::min(remaining, mSprites.max_size());

		std::memcpy(&mSprites[0], &mSpriteRenderCommands[mSpriteRenderCommands.size() - remaining], batchSize * sizeof(mSprites[0]));

		RenderBatch(batchSize);

		remaining -= batchSize;
	}
	mSpriteRenderCommands.clear();

	mTextureColor.Unbind(0);
	mTextureBack.Unbind(0);
	mShader.Unbind();
	mSamplerState.Unbind(0);
	DoGUI();
}

void JuicyRenderer::RenderBatch(std::size_t batchSize) {
	auto& context = MM::Get<Framework>().Context();

	std::sort(
	    mSprites.begin(), mSprites.begin() + batchSize, [](auto a, auto b) { return a.position.z > b.position.z; });
	mSpriteBuffer.SetData(mSprites.data(), batchSize * sizeof(mSprites[0]));

	mSpriteBuffer.Bind(sizeof(mSprites[0]), 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(batchSize, 0);
}

void JuicyRenderer::Submit(RCSprite renderCommand) {
	mSpriteRenderCommands.push_back(renderCommand);
}

void JuicyRenderer::DoGUI() {
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
}

void JuicyRenderer::UpdateConstantBuffer() {
	auto size = MM::Get<Window>().GetSize();

	mConstantBufferData.ProjectionMatrix = glm::perspectiveFovLH(glm::radians(80.f), size.x, size.y, 0.01f, 100.f);
	mConstantBufferData.Resolution       = glm::vec4(size.x, size.y, size.x / size.y, size.y / size.x);
	mConstantBufferData.Time             = glm::vec4((float)glfwGetTime());

	mConstantBuffer.SetData(mConstantBufferData);
}

}  // namespace JR