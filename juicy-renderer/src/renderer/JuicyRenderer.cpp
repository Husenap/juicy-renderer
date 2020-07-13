#include "JuicyRenderer.h"

#include "framework/Framework.h"
#include "framework/TextureManager.h"
#include "framework/Window.h"

namespace JR {
bool JuicyRenderer::Init() {
	if (!mShader.Load(Shader::Vertex | Shader::Geometry | Shader::Pixel, "assets/shaders/basic.hlsl")) {
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

	if (!mSamplerState.Create(SamplerState::Filter::Linear, SamplerState::Address::Wrap)) {
		return false;
	}

	return true;
}
void JuicyRenderer::Render() {
	mPremultipliedBlendState.Bind();
	mSamplerState.Bind(0);
	mShader.Bind();

	std::sort(std::execution::par, mSpriteRenderCommands.begin(), mSpriteRenderCommands.end(), [](auto a, auto b) {
		return a.position.z > b.position.z;
	});

	for (auto& sprite : mSpriteRenderCommands) {
		RenderSprite(sprite);
	}

	mSpriteRenderCommands.clear();

	mShader.Unbind();
	mSamplerState.Unbind(0);
}

void JuicyRenderer::RenderSprite(const RCSprite& sprite) {
	auto& context = MM::Get<Framework>().Context();

	auto& texture     = MM::Get<TextureManager>().GetTexture(sprite.texture);
	auto& backTexture = MM::Get<TextureManager>().GetTexture(sprite.backTexture);
	texture.Bind(0);
	backTexture.Bind(1);

	mSpriteBuffer.SetData(&sprite, sizeof(sprite));
	mSpriteBuffer.Bind(sizeof(sprite), 0);

	UpdateConstantBuffer(texture);
	mConstantBuffer.Bind(0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(1, 0);
}

void JuicyRenderer::Submit(RCSprite renderCommand) {
	mSpriteRenderCommands.push_back(renderCommand);
}

void JuicyRenderer::UpdateConstantBuffer(const Texture& texture) {
	auto size = MM::Get<Window>().GetSize();

	mConstantBufferData.ProjectionMatrix = glm::perspectiveFovLH(glm::radians(80.f), size.x, size.y, 0.1f, 1000.f);
	mConstantBufferData.Resolution       = glm::vec4(size.x, size.y, size.x / size.y, size.y / size.x);
	mConstantBufferData.Time             = glm::vec2((float)glfwGetTime());
	mConstantBufferData.Stretch          = texture.GetStretch();

	mConstantBuffer.SetData(mConstantBufferData);
}

}  // namespace JR