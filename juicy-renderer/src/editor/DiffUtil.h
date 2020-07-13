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

	template <typename T>
	static void Snapshot(const T& data) {
		Get().SnapshotInternal<T>(data);
	}

	template <typename T>
	static void CommitChanges(const T& data, const char* commitMessage) {
		Get().CommitChangesInternal<T>(data, commitMessage);
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
			return;
		}
		mEntity = entity;
	}

	template <typename T>
	void HandleTransactionInternal(const T& data, const char* commitMessage) {
		if (!mEntity) {
			return;
		}
		if (ImGui::IsItemActivated()) {
			SnapshotInternal(data);
		}
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			CommitChangesInternal(data, commitMessage);
		} else if (ImGui::IsItemDeactivated()) {
			mHasActiveTransaction = false;
		}
	}

	template <typename T>
	void SnapshotInternal(const T& data) {
		if (mHasActiveTransaction) {
			LOG_FATAL("Trying to create a new snapshot in the middle of another transaction!");
			return;
		}
		mHasActiveTransaction = true;
		mSnapshot.resize(sizeof(data));
		std::memcpy(mSnapshot.data(), &data, sizeof(data));
	}

	template <typename T>
	void CommitChangesInternal(const T& data, const char* commitMessage) {
		if (!mHasActiveTransaction) {
			LOG_FATAL("Trying to commit a change without any active transaction!");
			return;
		}
		std::vector<uint8_t> updatedData;
		updatedData.resize(sizeof(data));
		std::memcpy(updatedData.data(), &data, sizeof(data));

		MM::Get<TransactionManager>().RecordTransaction(
		    *mEntity, mSnapshot, updatedData, commitMessage, TypeId::Get<T>());

		mHasActiveTransaction = false;
	}

	std::vector<uint8_t> mSnapshot;
	std::optional<entt::entity> mEntity;
	bool mHasActiveTransaction;
};

}  // namespace JR