#include "TextureManager.h"

#include "editor/ContentManager.h"

namespace JR {

const JR::Texture& TextureManager::GetTexture(StringId id) {
	auto it = mTextureMap.find(id);
	if (it == mTextureMap.end()) {
		auto path = MM::Get<ContentManager>().GetPath(id);

		if (!path) {
			ValidateDefaultTexture();
			return mDefaultTexture;
		}

		if (path->extension() != ".png") {
			return mDefaultTexture;
		}

		Texture newTexture;
		if (!newTexture.CreateFromFile(path->string())) {
			LOG_ERROR("Failed to load texture from file: %s", path->string().c_str());
			ValidateDefaultTexture();
			return mDefaultTexture;
		}

		it = mTextureMap.emplace(id, newTexture).first;
	}

	return it->second;
}

void TextureManager::ValidateDefaultTexture() {
	if (mDefaultTexture.IsValid()) {
		return;
	}

	mDefaultTexture.CreateFromFile("assets/textures/default.png");
}

}  // namespace JR