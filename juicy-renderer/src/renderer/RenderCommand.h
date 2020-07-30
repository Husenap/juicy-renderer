#pragma once

namespace JR {

struct RCClearColor {
	glm::vec4 color;
};

struct RCSprite {
	glm::vec4 position;
	glm::vec4 uv;
	glm::vec4 tint;
	float blendMode;
	StringId texture;
	StringId backTexture;
};

struct RCLight {
	glm::vec2 position;
	float intensity;
	float size;
	glm::vec3 color;
	float isBackLight;
};

using RenderCommand = std::variant<RCSprite, RCClearColor, RCLight>;

}  // namespace JR