#pragma once

namespace JR {
class TypeId {
public:
	template <typename T>
	static uint32_t Get() {
		static uint32_t id = ++globalCounter;
		return id;
	}

	template <typename T>
	static uint32_t Get(const T&) {
		return Get<T>();
	}

private:
	static uint32_t globalCounter;
};
}  // namespace JR