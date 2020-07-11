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

	void Submit(RCSprite renderCommand);

private:
	void RenderBatch(std::size_t batchSize);
	void UpdateConstantBuffer();

	Shader mShader;
	std::array<RCSprite, 2048> mSprites;
	Buffer mSpriteBuffer;

	struct ConstantBufferData {
		glm::mat4 ProjectionMatrix;
		glm::vec4 Resolution;
		glm::vec4 Time;
	} mConstantBufferData;
	Buffer mConstantBuffer;

	Texture mTextureColor;
	Texture mTextureBack;

	BlendState mPremultipliedBlendState;
	SamplerState mSamplerState;

	std::vector<RCSprite> mSpriteRenderCommands;
};

}  // namespace JR
