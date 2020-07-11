#include "TransactionManager.h"

namespace JR {

void TransactionManager::RecordTransaction(entt::entity entity,
                                           std::vector<uint8_t> before,
                                           std::vector<uint8_t> after,
                                           const char* commitMessage) {
	char buf[3];
	std::string strBefore;
	std::string strAfter;

	for (auto i = 0; i < before.size(); ++i) {
		sprintf(buf, "%02X", before[i]);
		strBefore += buf;
		sprintf(buf, "%02X", after[i]);
		strAfter += buf;
	}
	LOG_INFO("=== %s ===", commitMessage);
	LOG_INFO("Before: %s", strBefore.c_str());
	LOG_INFO(" After: %s", strAfter.c_str());

	mUndoStack.emplace_back(
	    Transaction{.entity = entity, .before = before, .after = after, .commitMessage = commitMessage});
}

}  // namespace JR