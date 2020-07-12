#include "Scene.h"

#include "components/Components.h"
#include "framework/Framework.h"
#include "framework/Window.h"
#include "renderer/RenderCommand.h"

namespace JR {

using namespace Components;

Scene::Scene()
    : mEditor(mECS) {
	mEditor.Init<Identification, Transform, Sprite>();

	for (auto i = 0; i < 10; ++i) {
		auto entity = mECS.create();
		mECS.emplace<Identification>(entity, "entity_" + std::to_string(i));
		mECS.emplace<Transform>(entity, glm::vec3(-2 + i % 3, i / 3, 4.f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.f));

		switch (i % 4) {
		case 0:
			mECS.emplace<Sprite>(entity,
			                     glm::vec4(0.f, 0.f, 1.f, 1.f),
			                     glm::vec4(1.f),
			                     1.f,
			                     StringId::FromPath("groundprops.png"),
			                     StringId::FromPath("groundprops_back.png"));
			break;
		case 1:
			mECS.emplace<Sprite>(entity,
			                     glm::vec4(0.f, 0.f, 1.f, 1.f),
			                     glm::vec4(1.f),
			                     1.f,
			                     StringId::FromPath("groundsquare.png"),
			                     StringId::FromPath("groundsquare_back.png"));
			break;
		case 2:
			mECS.emplace<Sprite>(entity,
			                     glm::vec4(0.f, 0.f, 1.f, 1.f),
			                     glm::vec4(1.f),
			                     1.f,
			                     StringId::FromPath("pillar.png"),
			                     StringId::FromPath("pillar_back.png"));
			break;
		default:
			mECS.emplace<Sprite>(entity, glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec4(1.f), 1.f);
			break;
		}
	}
}

void Scene::Update(float time) {
	mCurrentTime = time;
	mDeltaTime   = mCurrentTime - mLastTime;
	mLastTime    = mCurrentTime;

	auto& renderer  = MM::Get<Framework>().Renderer();
	auto spriteView = mECS.view<Transform, Sprite>();
	for (auto entity : spriteView) {
		auto& transform = spriteView.get<Transform>(entity);
		auto& sprite    = spriteView.get<Sprite>(entity);

		renderer.Submit(RCSprite{.position    = glm::vec4(transform.position, 1.0),
		                         .uv          = sprite.uv,
		                         .tint        = sprite.tint,
		                         .blendMode   = sprite.blendMode,
		                         .texture     = sprite.texture,
		                         .backTexture = sprite.backTexture});
	}

	mEditor.Update();
}

}  // namespace JR
