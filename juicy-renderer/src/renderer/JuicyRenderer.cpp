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

	mVertices = {
	    {{-1.1f, 0.0f, 1.0f, 1.0f}, {0.0, 0.0, 0.5, 0.5}},
	    {{1.1f, 0.0f, 1.0f, 1.0f}, {0.5, 0.5, 1.0, 1.0}},
	    {{7.5f, 0.0f, 7.5f, 1.0f}, {0.0, 0.5, 0.5, 1.0}},
	};
	if (!mVertexBuffer.Create(CD3D11_BUFFER_DESC(
	        sizeof(VertexData) * 512, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE))) {
		LOG_ERROR("Failed to create Vertex Buffer!");
		return false;
	}
	mVertexBuffer.SetData(mVertices);

	mConstantBuffer.Create(CD3D11_BUFFER_DESC(
	    sizeof(ConstantBufferData), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE));

	return true;
}
void JuicyRenderer::Render() {
	auto& context = MM::Get<Framework>().Context();

	std::vector<VertexData> vertices = mVertices;
	std::sort(vertices.begin(), vertices.end(), [](auto a, auto b) { return a.position.z > b.position.z; });
	mVertexBuffer.SetData(vertices);

	mShader.Bind();
	mTextureColor.Bind(0);
	mTextureBack.Bind(1);

	UpdateConstantBuffer();
	mConstantBuffer.Bind(0);

	mVertexBuffer.Bind(sizeof(VertexData), 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(vertices.size(), 0);

	mTextureColor.Unbind(0);
	mTextureBack.Unbind(0);
	mShader.Unbind();

	DoGUI();
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

void JuicyRenderer::UpdateConstantBuffer() {
	auto size = MM::Get<Window>().GetSize();

	mConstantBufferData.ProjectionMatrix = glm::perspectiveFovLH(glm::radians(80.f), size.x, size.y, 0.01f, 100.f);
	mConstantBufferData.Resolution       = glm::vec4(size.x, size.y, size.x / size.y, size.y / size.x);

	mConstantBuffer.SetData(mConstantBufferData);
}

}  // namespace JR