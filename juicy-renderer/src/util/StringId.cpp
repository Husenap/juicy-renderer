#include "StringId.h"

namespace JR {

StringId::StringId()
    : mHash(0) {}

StringId::StringId(StringIdType id)
    : mHash(id) {}

StringId StringId::FromString(const std::string& str) {
	const static std::hash<std::string> stringHasher;
	return StringId(stringHasher(str));
}

StringId StringId::FromPath(const std::filesystem::path& path) {
	return StringId(std::filesystem::hash_value(path));
}

}  // namespace JR