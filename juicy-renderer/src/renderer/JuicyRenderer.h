#pragma once

#include "framework/Buffer.h"
#include "framework/Shader.h"
#include "framework/Texture.h"

namespace JR {

class JuicyRenderer {
public:
	bool Init();
	void Render();


private:
	void UpdateConstantBuffer();
	void DoGUI();

	Shader mShader;
	struct VertexData {
		glm::vec4 position;
		glm::vec4 uv;
		glm::vec4 tint  = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
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
};

}  // namespace JR
