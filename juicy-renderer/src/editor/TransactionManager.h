#pragma once

#include <entt/entity/entity.hpp>

namespace JR {

class TransactionManager : public Module {
	struct Transaction {
		entt::entity entity;
		std::vector<uint8_t> before;
		std::vector<uint8_t> after;
		std::string commitMessage;
	};

public:
	void RecordTransaction(entt::entity entity,
	                       std::vector<uint8_t> before,
	                       std::vector<uint8_t> after,
	                       const char* commitMessage);

	template <typename Func>
	void EnumerateUndoStack(Func func) const {
		static_assert(std::is_invocable_v<Func, const std::string&>);

		for (const auto& t : mUndoStack) {
			func(t.commitMessage);
		}
	}

private:
	std::vector<Transaction> mUndoStack;
};

}  // namespace JR