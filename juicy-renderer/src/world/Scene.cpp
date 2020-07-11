#include "Scene.h"

#include "components/Components.h"
#include "framework/Framework.h"
#include "framework/Window.h"
#include "renderer/RenderCommand.h"

namespace JR {

using namespace Components;

bool Scene::Init() {
	mLastTime = mCurrentTime = mDeltaTime = 0.f;

	for (auto i = 0; i < 10; ++i) {
		auto entity = mECS.create();
		mECS.emplace<Identification>(entity, "entity_" + std::to_string(i));
		mECS.emplace<Transform>(entity, glm::vec3(i, 0.f, 10.f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.f));
		mECS.emplace<Sprite>(entity, glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec4(1.f), 1.f);
	}

	mShowGUIToken = MM::Get<Window>().Subscribe<EventKeyPress>([&](const EventKeyPress& message) {
		if (message.key == GLFW_KEY_F11) {
			mShowGUI = !mShowGUI;
		}
	});

	mTransactionToken = MM::Get<TransactionManager>().Subscribe<EventTransaction>([&](const EventTransaction& message) {
		auto it = mTransactionHandlers.find(message.componentId);
		if (it == mTransactionHandlers.end()) {
			LOG_ERROR("Failed to find transaction handler for component with id: %d!", message.componentId);
			return;
		}

		it->second(message);
	});

	RegisterTransactionHandlers<Identification, Transform, Sprite>();

	return true;
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

		renderer.Submit(RCSprite{.position  = glm::vec4(transform.position, 1.0),
		                         .uv        = sprite.uv,
		                         .tint      = sprite.tint,
		                         .blendMode = sprite.blendMode});
	}

	UpdateEditor();
}

void Scene::UpdateEditor() {
	if (!mShowGUI) {
		return;
	}

	DrawDockSpace();
	DrawInspector();
	DrawHierarchy();
	DrawHistory();

	static bool demo = true;
	if (demo) {
		ImGui::ShowDemoWindow(&demo);
	}
}

void Scene::DrawDockSpace() {
	ImGuiWindowFlags dockSpaceWindowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking |
	                                        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
	                                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
	                                        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("DOCK_SPACE", nullptr, dockSpaceWindowFlags);

	ImGui::SetNextWindowBgAlpha(0.f);
	ImGui::DockSpace(ImGui::GetID("DOCK_SPACE_WINDOW"), {0.f, 0.f}, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
	ImGui::PopStyleVar(2);
}

void Scene::DrawInspector() {
	if (ImGui::Begin("Inspector")) {
		if (mSelectedEntity) {
			DrawEntityComponents<Identification, Transform, Sprite>(*mSelectedEntity);
		}
	}
	ImGui::End();
}

void Scene::DrawHierarchy() {
	if (ImGui::Begin("Hierarchy")) {
		const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
		                                     ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

		auto view = mECS.view<Identification>();

		std::optional<entt::entity> clickedEntity;
		for (auto entity : view) {
			auto& identification = view.get<Identification>(entity);

			ImGuiTreeNodeFlags nodeFlags = baseFlags;
			if (mSelectedEntity && mSelectedEntity == entity) {
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			bool nodeOpen = ImGui::TreeNodeEx((void*)entity, nodeFlags, "%s", identification.name);

			if (ImGui::IsItemClicked()) {
				clickedEntity = entity;
			}

			if (nodeOpen) {
				ImGui::TreePop();
			}
		}

		if (clickedEntity) {
			mSelectedEntity = clickedEntity;
		}
	}
	ImGui::End();
}

void Scene::DrawHistory() {
	if (ImGui::Begin("History")) {
		const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth |
		                                     ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen |
		                                     ImGuiTreeNodeFlags_Bullet;

		MM::Get<TransactionManager>().EnumerateUndoStack([](const std::string& commitMessage, bool isActive) {
			ImGuiTreeNodeFlags nodeFlags = baseFlags;
			if (isActive) {
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}
			ImGui::TreeNodeEx(commitMessage.c_str(), nodeFlags);
		});
		MM::Get<TransactionManager>().EnumerateRedoStack([](const std::string& commitMessage) {
			ImGuiTreeNodeFlags nodeFlags = baseFlags;
			ImGui::TreeNodeEx(commitMessage.c_str(), nodeFlags);
		});
	}
	ImGui::End();
}

}  // namespace JR
