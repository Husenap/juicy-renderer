#pragma once

#include "Texture.h"

namespace JR {

class TextureManager : public Module {
public:
	const Texture& GetTexture(StringId id);

private:
	void ValidateDefaultTexture();

	Texture mDefaultTexture;

	std::map<StringId, Texture> mTextureMap;
};

}  // namespace JR