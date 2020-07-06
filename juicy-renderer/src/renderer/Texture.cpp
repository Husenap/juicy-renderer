#include "Texture.h"

#include <stb_image.h>

#include "Framework.h"

namespace JR {

bool Texture::CreateFromFile(const std::string& filepath) {
	const auto imageLoader = [=]() {
		int32_t w, h, comp;
		uint8_t* rawData = stbi_load(filepath.c_str(), &w, &h, &comp, STBI_rgb_alpha);

		if (!rawData) {
			LOG_ERROR("Failed to load image: %s", filepath.c_str());
			return false;
		}

		if (!Create(w, h, DXGI_FORMAT_R8G8B8A8_UNORM, rawData)) {
			LOG_ERROR("Failed to create image: %s", filepath.c_str());
			stbi_image_free(rawData);
			return false;
		}

		stbi_image_free(rawData);

		return true;
	};

	if (!imageLoader()) {
		return false;
	}

	MM::Get<FileWatcher>().Watch(filepath, [=]() { imageLoader(); });

	return true;
}

bool Texture::Create(uint32_t w, uint32_t h, DXGI_FORMAT format, uint8_t* imageData) {
	return Create(TextureCreateDesc{
	    .width  = w,
	    .height = h,
	    .data   = imageData,
	    .format = format,
	});
}

bool Texture::Create(const TextureCreateDesc& createDesc) {
	CD3D11_TEXTURE2D_DESC textureDesc(
	    createDesc.format, createDesc.width, createDesc.height, 1, 1, D3D11_BIND_SHADER_RESOURCE);

	D3D11_SUBRESOURCE_DATA initialData{.pSysMem     = createDesc.data,
	                                   .SysMemPitch = createDesc.width * sizeof(uint8_t) * 4};

	HRESULT hr = MM::Get<Framework>().Device()->CreateTexture2D(
	    &textureDesc, createDesc.data ? &initialData : nullptr, &mTexture);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create texture!");
		return false;
	}

	CreateFromTexture(mTexture);

	return true;
}

void Texture::CreateFromTexture(ComPtr<ID3D11Texture2D> texture) {
	MM::Get<Framework>().Device()->CreateShaderResourceView(texture.Get(), nullptr, &mShaderResourceView);
}

void Texture::Bind(uint32_t slot) const {
	MM::Get<Framework>().Context()->PSSetShaderResources(slot, 1, mShaderResourceView.GetAddressOf());
}

void Texture::Unbind(uint32_t slot) const {
	ID3D11ShaderResourceView* nullSRV[] = {nullptr};
	MM::Get<Framework>().Context()->PSSetShaderResources(slot, 1, nullSRV);
}

}  // namespace JR