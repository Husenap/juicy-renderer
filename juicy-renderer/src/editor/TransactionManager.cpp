#include "TransactionManager.h"

namespace JR {

TransactionManager::TransactionManager() {
	mKeyPressToken = MM::Get<Window>().Subscribe<EventKey>([&](const auto& message) {
		if (message.action == GLFW_RELEASE || message.mods != GLFW_MOD_CONTROL) {
			return;
		}
		if (message.key == GLFW_KEY_Z) {
			if (mUndoStack.empty()) {
				return;
			}

			const Transaction& transaction = mRedoStack.emplace_back(mUndoStack.back());
			mUndoStack.pop_back();

			std::visit(
			    overloaded{
			        [&](ComponentAction a) {
				        Emit(EventComponentTransaction{
				            .data = a.before, .entity = a.entity, .componentId = a.componentId});
			        },
			        [&](AddComponentAction a) {
				        Emit(EventRemoveComponent{.entity = a.entity, .componentId = a.componentId});
			        },
			        [&](RemoveComponentAction a) {
				        Emit(EventAddComponent{.data = a.data, .entity = a.entity, .componentId = a.componentId});
			        },
			        [&](AddEntityAction a) { Emit(EventRemoveEntity{.entity = a.entity}); },
			        [&](RemoveEntityAction a) {
				        Emit(EventAddEntity{.entity = a.entity, .componentData = a.componentData});
			        },
			    },
			    transaction.action);
		} else if (message.key == GLFW_KEY_Y) {
			if (mRedoStack.empty()) {
				return;
			}

			const Transaction& transaction = mUndoStack.emplace_back(mRedoStack.back());
			mRedoStack.pop_back();

			std::visit(overloaded{
			               [&](ComponentAction a) {
				               Emit(EventComponentTransaction{
				                   .data = a.after, .entity = a.entity, .componentId = a.componentId});
			               },
			               [&](AddComponentAction a) {
				               Emit(EventAddComponent{.entity = a.entity, .componentId = a.componentId});
			               },
			               [&](RemoveComponentAction a) {
				               Emit(EventRemoveComponent{.entity = a.entity, .componentId = a.componentId});
			               },
			               [&](AddEntityAction a) {
				               Emit(EventAddEntity{.entity = a.entity, .componentData = a.componentData});
			               },
			               [&](RemoveEntityAction a) { Emit(EventRemoveEntity{.entity = a.entity}); },
			           },
			           transaction.action);
		}
	});
}

void TransactionManager::RecordComponentTransaction(entt::entity entity,
                                                    std::vector<uint8_t> before,
                                                    std::vector<uint8_t> after,
                                                    const char* commitMessage,
                                                    uint32_t componentId) {
	mUndoStack.emplace_back(Transaction{
	    .action = ComponentAction{.before = before, .after = after, .entity = entity, .componentId = componentId},
	    .commitMessage = commitMessage});

	mRedoStack.clear();
}

void TransactionManager::AddComponent(entt::entity entity, const char* commitMessage, uint32_t componentId) {
	mUndoStack.emplace_back(Transaction{
	    .action        = AddComponentAction{.entity = entity, .componentId = componentId},
	    .commitMessage = commitMessage,
	});

	Emit(EventAddComponent{.entity = entity, .componentId = componentId});

	mRedoStack.clear();
}

void TransactionManager::RemoveComponent(entt::entity entity,
                                         std::vector<uint8_t> data,
                                         const char* commitMessage,
                                         uint32_t componentId) {
	mUndoStack.emplace_back(Transaction{
	    .action        = RemoveComponentAction{.data = data, .entity = entity, .componentId = componentId},
	    .commitMessage = commitMessage,
	});

	Emit(EventRemoveComponent{.entity = entity, .componentId = componentId});

	mRedoStack.clear();
}

void TransactionManager::AddEntity(entt::entity entity,
                                   const char* commitMessage,
                                   std::unordered_map<uint32_t, std::vector<uint8_t>> snapshot) {
	mUndoStack.emplace_back(Transaction{
	    .action        = AddEntityAction{.entity = entity, .componentData = snapshot},
	    .commitMessage = commitMessage,
	});

	mRedoStack.clear();
}

void TransactionManager::RemoveEntity(entt::entity entity,
                                      const char* commitMessage,
                                      std::unordered_map<uint32_t, std::vector<uint8_t>> snapshot) {
	mUndoStack.emplace_back(Transaction{
	    .action        = RemoveEntityAction{.entity = entity, .componentData = snapshot},
	    .commitMessage = commitMessage,
	});

	Emit(EventRemoveEntity{.entity = entity});

	mRedoStack.clear();
}

}  // namespace JR