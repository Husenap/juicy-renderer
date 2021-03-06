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

	static void FlushChanges() {
		if (Get().mSnapshotCallback) {
			Get().mSnapshotCallback();		
			Get().mSnapshotCallback = {};
		}
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

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			CommitChangesInternal(data, commitMessage);
		}

		if (ImGui::IsItemDeactivated()) {
			mHasActiveTransaction = false;
		}

		if (ImGui::IsItemActivated()) {
			mSnapshotCallback = [&]() {
				SnapshotInternal(data);
			};
		}
	}

	template <typename T>
	void SnapshotInternal(const T& data) {
		if (mHasActiveTransaction) {
			LOG_ERROR("Trying to create a new snapshot in the middle of another transaction!");
			return;
		}
		mHasActiveTransaction = true;
		mSnapshot.resize(sizeof(data));
		std::memcpy(mSnapshot.data(), &data, sizeof(data));
	}

	template <typename T>
	void CommitChangesInternal(const T& data, const char* commitMessage) {
		if (!mHasActiveTransaction) {
			LOG_ERROR("Trying to commit a change without any active transaction!");
			return;
		}
		std::vector<uint8_t> updatedData;
		updatedData.resize(sizeof(data));
		std::memcpy(updatedData.data(), &data, sizeof(data));

		MM::Get<TransactionManager>().RecordComponentTransaction(
		    *mEntity, mSnapshot, updatedData, commitMessage, Components::ComponentTypeId::Get<T>());

		mHasActiveTransaction = false;
	}

	std::vector<uint8_t> mSnapshot;
	std::optional<entt::entity> mEntity;
	bool mHasActiveTransaction;
	std::function<void()> mSnapshotCallback;
};

}  // namespace JR