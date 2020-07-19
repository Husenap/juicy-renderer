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

using RenderCommand = std::variant<RCSprite, RCClearColor>;

}  // namespace JR