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
			Emit(EventTransaction{
			    .entity = transaction.entity, .data = transaction.before, .componentId = transaction.componentId});
		} else if (message.key == GLFW_KEY_Y) {
			if (mRedoStack.empty()) {
				return;
			}

			const Transaction& transaction = mUndoStack.emplace_back(mRedoStack.back());
			mRedoStack.pop_back();
			Emit(EventTransaction{
			    .entity = transaction.entity, .data = transaction.after, .componentId = transaction.componentId});
		}
	});
}

void TransactionManager::RecordTransaction(entt::entity entity,
                                           std::vector<uint8_t> before,
                                           std::vector<uint8_t> after,
                                           const char* commitMessage,
                                           uint32_t componentId) {
	mUndoStack.emplace_back(Transaction{.entity        = entity,
	                                    .before        = before,
	                                    .after         = after,
	                                    .commitMessage = commitMessage,
	                                    .componentId   = componentId});

	mRedoStack.clear();
}

}  // namespace JR