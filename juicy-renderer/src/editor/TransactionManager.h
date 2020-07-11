#pragma once

#include <entt/entity/entity.hpp>

#include "framework/Window.h"

namespace JR {

struct EventTransaction {
	entt::entity entity;
	std::vector<uint8_t> data;
	uint32_t componentId;
};

class TransactionManager : public Module, public MessageEmitter {
	struct Transaction {
		entt::entity entity;
		std::vector<uint8_t> before;
		std::vector<uint8_t> after;
		std::string commitMessage;
		uint32_t componentId;
	};

public:
	TransactionManager();

	void RecordTransaction(entt::entity entity,
	                       std::vector<uint8_t> before,
	                       std::vector<uint8_t> after,
	                       const char* commitMessage,
	                       uint32_t componentId);

	template <typename Func>
	void EnumerateUndoStack(Func func) const {
		static_assert(std::is_invocable_v<Func, const std::string&, bool>);

		if (mUndoStack.empty()) {
			return;
		}

		for (auto i = 0; i < mUndoStack.size(); ++i) {
			const auto& t = mUndoStack[i];
			func(t.commitMessage, i == mUndoStack.size() - 1);
		}
	}
	template <typename Func>
	void EnumerateRedoStack(Func func) const {
		static_assert(std::is_invocable_v<Func, const std::string&>);

		if (mRedoStack.empty()) {
			return;
		}

		for (auto i = mRedoStack.size(); i > 0; --i) {
			const auto& t = mRedoStack[i - 1];
			func(t.commitMessage);
		}
	}

private:
	std::vector<Transaction> mUndoStack;
	std::vector<Transaction> mRedoStack;

	MessageToken mKeyPressToken;
};

}  // namespace JR