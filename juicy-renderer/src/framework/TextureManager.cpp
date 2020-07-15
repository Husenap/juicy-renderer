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

		auto textureLoader = [=]() {
			Texture newTexture;
			if (!newTexture.CreateFromFile(path->string())) {
				return false;
			}
			mTextureMap[id] = newTexture;
			return true;
		};

		if (!textureLoader()) {
			LOG_ERROR("Failed to load texture from file: %s", path->string().c_str());
			ValidateDefaultTexture();
			return mDefaultTexture;
		}

		MM::Get<FileWatcher>().Watch(path->string(), [=]() { textureLoader(); });
		it = mTextureMap.find(id);
	}

	return it->second;
}

const Texture& TextureManager::GetDefaultTexture() {
	ValidateDefaultTexture();
	return mDefaultTexture;
}

void TextureManager::ValidateDefaultTexture() {
	if (mDefaultTexture.IsValid()) {
		return;
	}

	mDefaultTexture.CreateFromFile("assets/textures/default.png");
}

}  // namespace JR