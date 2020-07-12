#pragma once

#include <entt/entity/registry.hpp>

#include "editor/Editor.h"

namespace JR {
class Scene {
public:
	Scene();
	void Update(float time);

private:
	entt::registry mECS;

	float mCurrentTime = 0.f;
	float mLastTime    = 0.f;
	float mDeltaTime   = 0.f;

	Editor mEditor;
};
}  // namespace JR
