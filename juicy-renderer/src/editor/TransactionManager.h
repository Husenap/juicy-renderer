#pragma once

#include <entt/entity/entity.hpp>

#include "framework/Window.h"

namespace JR {

struct EventComponentTransaction {
	std::vector<uint8_t> data;
	entt::entity entity;
	uint32_t componentId;
};
struct EventAddComponent {
	std::vector<uint8_t> data;
	entt::entity entity;
	uint32_t componentId;
};
struct EventRemoveComponent {
	entt::entity entity;
	uint32_t componentId;
};
struct EventAddEntity {
	entt::entity entity;
	std::unordered_map<uint32_t, std::vector<uint8_t>> componentData;
};
struct EventRemoveEntity {
	entt::entity entity;
};

class TransactionManager : public Module, public MessageEmitter {
	struct ComponentAction {
		std::vector<uint8_t> before;
		std::vector<uint8_t> after;
		entt::entity entity;
		uint32_t componentId;
	};
	struct AddComponentAction {
		entt::entity entity;
		uint32_t componentId;
	};
	struct RemoveComponentAction {
		std::vector<uint8_t> data;
		entt::entity entity;
		uint32_t componentId;
	};
	struct AddEntityAction {
		entt::entity entity;
		std::unordered_map<uint32_t, std::vector<uint8_t>> componentData;
	};
	struct RemoveEntityAction {
		entt::entity entity;
		std::unordered_map<uint32_t, std::vector<uint8_t>> componentData;
	};

	using Action =
	    std::variant<ComponentAction, AddComponentAction, RemoveComponentAction, AddEntityAction, RemoveEntityAction>;

	struct Transaction {
		Action action;
		std::string commitMessage;
	};

public:
	TransactionManager();

	void RecordComponentTransaction(entt::entity entity,
	                                std::vector<uint8_t> before,
	                                std::vector<uint8_t> after,
	                                const char* commitMessage,
	                                uint32_t componentId);

	void AddComponent(entt::entity entity, const char* commitMessage, uint32_t componentId);
	void RemoveComponent(entt::entity entity,
	                     std::vector<uint8_t> data,
	                     const char* commitMessage,
	                     uint32_t componentId);

	void AddEntity(entt::entity entity,
	               const char* commitMessage,
	               std::unordered_map<uint32_t, std::vector<uint8_t>> snapshot);
	void RemoveEntity(entt::entity entity,
	                  const char* commitMessage,
	                  std::unordered_map<uint32_t, std::vector<uint8_t>> snapshot);

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