#pragma once

namespace JR {

using StringIdType = uint64_t;

class StringId {
public:
	StringId();
	StringId(StringIdType id);
	static StringId FromString(const std::string& str);
	static StringId FromPath(const std::filesystem::path& path);

	operator StringIdType() const { return mHash; }

private:
	StringIdType mHash;
};

}  // namespace JR