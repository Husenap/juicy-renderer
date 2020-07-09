#pragma once

#include "Module.h"

namespace JR {
class ModuleManager {
public:
	template <typename T, typename... Args>
	static void AddModule(Args&&... args) {
		RegisterModule<T>(std::forward<Args>(args)...);
		Load<T>();
	}

	template <typename T, typename... Args>
	static void RegisterModule(Args&&... args) {
		const auto id       = TypeId::Get<T>();
		Get().mRegistry[id] = [args...]() { return ModuleEntry{.mModule = std::make_unique<T>(args...)}; };
	}

	template <typename T>
	static bool IsLoaded() {
		return Get().IsLoaded(TypeId::Get<T>());
	}

	template <typename T>
	static bool IsRegistered() {
		return Get().IsRegistered(TypeId::Get<T>());
	}

	template <typename T>
	static T& Get() {
		return static_cast<T&>(*Get().mModules.at(TypeId::Get<T>()).mModule);
	}

	template <typename T>
	static void Load() {
		Get().Load(TypeId::Get<T>());
	}

	template <typename T>
	static void Unload() {
		Get().Unload(TypeId::Get<T>());
	}

	static void UnloadAll() { Get().mModules.clear(); }

private:
	static ModuleManager& Get() {
		static ModuleManager instance;
		return instance;
	}

	bool IsLoaded(uint32_t id) const { return mModules.find(id) != mModules.end(); }

	bool IsRegistered(uint32_t id) const { return mRegistry.find(id) != mRegistry.end(); }

	void Load(uint32_t id) {
		if (!IsRegistered(id)) {
			throw std::runtime_error("Module is not registered!");
		}

		if (!IsLoaded(id)) {
			const auto& ModuleCreator = mRegistry.at(id);
			mModules.insert(std::make_pair(id, ModuleCreator()));
		}
	}

	void Unload(uint32_t id) {
		if (!IsRegistered(id)) {
			throw std::runtime_error("Module is not registered!");
		}

		if (IsLoaded(id)) {
			mModules.erase(id);
		}
	}

	struct ModuleEntry {
		std::unique_ptr<Module> mModule;
	};

	std::map<uint32_t, std::function<ModuleEntry()>> mRegistry;
	std::map<uint32_t, const ModuleEntry> mModules;
};

using MM = ModuleManager;

}  // namespace JR