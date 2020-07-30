#include "Scene.h"

#include "components/Components.h"
#include "framework/Framework.h"
#include "framework/Window.h"
#include "renderer/RenderCommand.h"

namespace JR {

using namespace Components;

Scene::Scene()
    : mEditor(*this, mECS) {
	mEditor.Init<Identification, Transform, Sprite, Light>();

	mBackgroundColor = {0.f, 0.f, 0.f, 1.f};
}

void Scene::Update(float time) {
	mEditor.Update();

	mCurrentTime = time;
	mDeltaTime   = mCurrentTime - mLastTime;
	mLastTime    = mCurrentTime;

	auto& renderer = MM::Get<Framework>().Renderer();

	renderer.Submit(RCClearColor{.color = mBackgroundColor});

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

	auto lightView = mECS.view<Transform, Light>();
	for (auto entity : lightView) {
		auto& transform = lightView.get<Transform>(entity);
		auto& light     = lightView.get<Light>(entity);

		renderer.Submit(RCLight{
		    .position    = transform.position,
		    .intensity   = light.intensity,
		    .size        = light.size,
		    .color       = light.color,
		    .isBackLight = static_cast<float>(light.direction == Components::LightDirection::Back),
		});
	}
}

}  // namespace JR
