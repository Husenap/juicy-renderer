#pragma once

#include "editor/Editor.h"

namespace JR {
class Scene {
public:
	Scene();
	void Update(float time);

private:
	ECS mECS;

	float mCurrentTime = 0.f;
	float mLastTime    = 0.f;
	float mDeltaTime   = 0.f;

	Editor mEditor;
};
}  // namespace JR
