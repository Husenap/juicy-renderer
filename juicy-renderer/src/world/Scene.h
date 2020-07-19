#pragma once

#include "editor/Editor.h"

namespace JR {
class Scene {
public:
	Scene();

	void Update(float time);

private:
	friend class Editor;

	ECS mECS;

	float mCurrentTime = 0.f;
	float mLastTime    = 0.f;
	float mDeltaTime   = 0.f;

	glm::vec4 mBackgroundColor;

	Editor mEditor;
};
}  // namespace JR
