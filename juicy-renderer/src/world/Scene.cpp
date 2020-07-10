#include "Scene.h"

#include "framework/Framework.h"
#include "renderer/RenderCommand.h"

namespace JR {

struct Transform {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
};

struct Sprite {
	glm::vec4 uv;
	glm::vec4 tint;
	float blendMode;
};

bool Scene::Init() {
	mLastTime = mCurrentTime = mDeltaTime = 0.f;

	for (auto i = 0; i < 512; ++i) {
		auto entity = mECS.create();
		mECS.emplace<Transform>(entity, glm::vec3(i*0.00000001f+0.01f, 0.f, 0.f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.f));
		mECS.emplace<Sprite>(entity, glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec4(1.f), 1.f);
	}

	return true;
}

void Scene::Update(float time) {
	mCurrentTime = time;
	mDeltaTime   = mCurrentTime - mLastTime;
	mLastTime    = mCurrentTime;

	static constexpr float sigma = 10.f;
	static constexpr float rho   = 28.f;
	static constexpr float beta  = 8 / 3.f;

	auto transformView = mECS.view<Transform>();
	std::for_each(std::execution::par, transformView.begin(), transformView.end(), [&](auto entity) {
		auto& transform = transformView.get<Transform>(entity);

		auto& p = transform.position;
		p += std::min(mDeltaTime, 1.f/60.f) * glm::vec3(sigma * (p.y - p.x), p.x * (rho - p.z) - p.y, p.x * p.y - beta * p.z);
	});


	auto& renderer  = MM::Get<Framework>().Renderer();
	auto spriteView = mECS.view<Transform, Sprite>();
	for (auto entity : spriteView) {
		auto& transform = spriteView.get<Transform>(entity);
		auto& sprite    = spriteView.get<Sprite>(entity);

		renderer.Submit(RCSprite{.position  = glm::vec4(transform.position, 1.0),
		                         .uv        = sprite.uv,
		                         .tint      = sprite.tint,
		                         .blendMode = sprite.blendMode});
	}

	if (ImGui::Begin("VERTICES")) {
		for (auto entity : spriteView) {
			auto& transform = spriteView.get<Transform>(entity);
			auto& sprite    = spriteView.get<Sprite>(entity);

			ImGui::PushID(ImGui::GetID(&sprite));
			ImGui::DragFloat3("position", &transform.position.x, 0.05f);
			ImGui::DragFloat4("uv", &sprite.uv.x, 0.05f);
			ImGui::ColorEdit4("tint", &sprite.tint.x);
			ImGui::SliderFloat("Blend Mode", &sprite.blendMode, 0.0f, 1.0f, "Additive - %.3f - Alpha Blend");
			ImGui::Separator();
			ImGui::PopID();
		}
	}
	ImGui::End();
}

}  // namespace JR
