#include "Scene.h"

namespace JR {

struct Transform {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
};

struct Sprite {
	glm::vec4 tint;
};

bool Scene::Init() {
	mLastTime = mCurrentTime = mDeltaTime = 0.f;

	for (auto i = 0; i < 10; ++i) {
		auto entity = mECS.create();
		mECS.emplace<Transform>(entity, glm::vec3(-5 + i, 0.f, 10.f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.f));
		mECS.emplace<Sprite>(entity);
	}

	return true;
}

void Scene::Update(float time) {
	mCurrentTime = time;
	mDeltaTime   = mCurrentTime - mLastTime;
	mLastTime    = mCurrentTime;

	/*
	auto view = mECS.view<Transform>();
	for (auto entity : view) {
		auto& transform = view.get<Transform>(entity);
	}
	*/
}

}  // namespace JR
