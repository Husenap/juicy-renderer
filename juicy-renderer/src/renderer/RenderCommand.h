#pragma once

namespace JR {

struct RCSprite {
	glm::vec4 position;
	glm::vec4 uv;
	glm::vec4 tint;
	float blendMode;
};

using RenderCommand = std::variant<RCSprite>;

}  // namespace JR