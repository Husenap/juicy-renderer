#pragma once

#include "RenderCommand.h"
#include "framework/BlendState.h"
#include "framework/Buffer.h"
#include "framework/SamplerState.h"
#include "framework/Shader.h"
#include "framework/Texture.h"

namespace JR {

class JuicyRenderer {
public:
	bool Init();
	void Render();

	void Submit(RCSprite sprite);
	void Submit(RCLight light);

	void SetTime(float time);

private:
	void RenderSprite(const RCSprite& sprite);
	void UpdateConstantBuffer(const Texture& texture);
	void UpdateLightBuffer();

	Shader mShader;
	std::array<RCSprite, 2048> mSprites;
	Buffer mSpriteBuffer;

	struct ConstantBufferData {
		glm::mat4 ProjectionMatrix;
		glm::vec4 Resolution;
		glm::vec2 Stretch;
		float Time;
		float _padding[1];
	} mConstantBufferData;
	Buffer mConstantBuffer;

	struct LightBufferData {
		RCLight Lights[64];
		int NumLights;
		float _padding[3];
	} mLightBufferData;
	Buffer mLightBuffer;

	BlendState mPremultipliedBlendState;
	SamplerState mSamplerState;

	std::vector<RCSprite> mSpriteRenderCommands;
	std::vector<RCLight> mLightRenderCommands;
};

}  // namespace JR
