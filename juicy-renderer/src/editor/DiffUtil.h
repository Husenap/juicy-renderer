#pragma once

#include "TransactionManager.h"

namespace JR {

class DiffUtil {
public:
	static void SetActiveEntity(entt::entity entity) { Get().SetActiveEntityInternal(entity); }

	template <typename T>
	static void HandleTransaction(const T& data, const char* commitMessage) {
		Get().HandleTransactionInternal<T>(data, commitMessage);
	}

private:
	DiffUtil()
	    : mHasActiveTransaction(false) {}

	static DiffUtil& Get() {
		static DiffUtil diff;
		return diff;
	}

	void SetActiveEntityInternal(entt::entity entity) {
		if (mEntity && mEntity == entity) {
			return;
		}
		if (mHasActiveTransaction) {
			LOG_FATAL("Trying to change entity while a transaction is active!");
		}
		mEntity = entity;
	}

	template <typename T>
	void HandleTransactionInternal(const T& data, const char* commitMessage) {
		if (!mEntity) {
			return;
		}
		if (ImGui::IsItemActivated()) {
			if (mHasActiveTransaction) {
				LOG_FATAL("Trying to create a new snapshot in the middle of another transaction!");
				return;
			}
			mHasActiveTransaction = true;
			Snapshot(data);
		}
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			if (!mHasActiveTransaction) {
				LOG_FATAL("Trying to commit a change without any active transaction!");
				return;
			}
			CommitChanges(data, commitMessage);
			mHasActiveTransaction = false;
		} else if (ImGui::IsItemDeactivated()) {
			mHasActiveTransaction = false;
		}
	}

	template <typename T>
	void Snapshot(const T& data) {
		mSnapshot.resize(sizeof(data));
		std::memcpy(mSnapshot.data(), &data, sizeof(data));
	}

	template <typename T>
	void CommitChanges(const T& data, const char* commitMessage) {
		std::vector<uint8_t> updatedData;
		updatedData.resize(sizeof(data));
		std::memcpy(updatedData.data(), &data, sizeof(data));

		MM::Get<TransactionManager>().RecordTransaction(*mEntity, mSnapshot, updatedData, commitMessage);
	}

	std::vector<uint8_t> mSnapshot;
	std::optional<entt::entity> mEntity;
	bool mHasActiveTransaction;
};

}  // namespace JR