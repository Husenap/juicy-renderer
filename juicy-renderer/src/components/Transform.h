#pragma once

namespace JR::Components {

class DiffUtil {
public:
	template <typename T>
	void HandleTransaction(const T& data, const char* commitMessage) {
		if (ImGui::IsItemActivated()) {
			Snapshot(data);
		}
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			CommitChanges(commitMessage);
		}
	}

private:
	template <typename T>
	void Snapshot(const T& data) {
		mSnapshot.resize(sizeof(data));
		std::memcpy(mSnapshot.data(), &data, sizeof(data));
		mSource = reinterpret_cast<const uint8_t*>(&data);
	}

	void CommitChanges(const char* commitMessage) {
		char buf[3];
		std::string before;
		std::string after;
		for (auto i = 0; i < mSnapshot.size(); ++i) {
			sprintf(buf, "%02X", mSnapshot[i]);
			before += buf;
			sprintf(buf, "%02X", mSource[i]);
			after += buf;
		}
		LOG_INFO("=== %s ===", commitMessage);
		LOG_INFO("Before: %s", before.c_str());
		LOG_INFO(" After: %s", after.c_str());
	}

	std::vector<uint8_t> mSnapshot;
	const uint8_t* mSource;
};

struct Transform {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
};

void View(Transform& transform) {
	static DiffUtil diff;

	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::DragFloat3("position", &transform.position.x, 0.05f);
		diff.HandleTransaction(transform, "Changed position");

		ImGui::DragFloat3("scale", &transform.scale.x, 0.05f);
		diff.HandleTransaction(transform, "Changed scale");

		ImGui::DragFloat3("rotation", &transform.rotation.x, 0.05f);
		diff.HandleTransaction(transform, "Changed rotation");
	}
}

}  // namespace JR::Components