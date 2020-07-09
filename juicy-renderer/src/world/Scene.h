#pragma once

#include <entt/entity/registry.hpp>

namespace JR {
class Scene {
public:
	bool Init();
	void Update(float time);

private:
	entt::registry mECS;

	float mCurrentTime;
	float mLastTime;
	float mDeltaTime;
};
}  // namespace JR
