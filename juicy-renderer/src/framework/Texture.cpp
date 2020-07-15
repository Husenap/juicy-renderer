#include "Texture.h"

#include <stb_image.h>

#include "Framework.h"

namespace JR {

bool Texture::CreateFromFile(const std::string& filepath) {
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
}

bool Texture::Create(uint32_t w, uint32_t h, DXGI_FORMAT format, uint8_t* imageData) {
	return Create(TextureCreateDesc{
	    .width = w, .height = h, .data = imageData, .format = format, .textureType = TextureType::ShaderResource});
}

bool Texture::Create(const TextureCreateDesc& createDesc) {
	mTextureType = createDesc.textureType;

	D3D11_TEXTURE2D_DESC textureDesc{
	    .Width  = createDesc.width,
	    .Height = createDesc.height,
	    .MipLevels = 0,
		.ArraySize = 1,
	    .Format    = createDesc.format,
	    .SampleDesc = {.Count = 1, .Quality = 0},
		.Usage = D3D11_USAGE_DEFAULT,
	    .BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		.CPUAccessFlags = 0,
	    .MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS,
	};

	HRESULT hr = MM::Get<Framework>().Device()->CreateTexture2D(&textureDesc, nullptr, &mTexture);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create texture!");
		return false;
	}

	if (createDesc.data) {
		std::transform(std::execution::par,
		               (uint32_t*)createDesc.data,
		               (uint32_t*)createDesc.data + createDesc.width * createDesc.height,
		               (uint32_t*)createDesc.data,
		               [](uint32_t c) -> uint32_t {
			               float r, g, b, a;
			               a = (c >> 24) / 255.f;
			               b = (c << 8 >> 24) / 255.f;
			               g = (c << 16 >> 24) / 255.f;
			               r = (c << 24 >> 24) / 255.f;

			               r *= a;
			               g *= a;
			               b *= a;

			               return (uint32_t(a * 255.f) << 24) | (uint32_t(b * 255.f) << 16) |
			                      (uint32_t(g * 255.f) << 8) | (uint32_t(r * 255.f));
		               });

		MM::Get<Framework>().Context()->UpdateSubresource(
		    mTexture.Get(), 0, NULL, createDesc.data, createDesc.width * sizeof(uint8_t) * 4, 0);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{.Format        = createDesc.format,
	                                        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
	                                        .Texture2D     = {
                                                .MostDetailedMip = 0,
                                                .MipLevels       = (UINT)-1,
                                            }};

	MM::Get<Framework>().Device()->CreateShaderResourceView(mTexture.Get(), &srvDesc, &mShaderResourceView);

	if (mTextureType == TextureType::RenderTarget) {
		MM::Get<Framework>().Device()->CreateRenderTargetView(mTexture.Get(), nullptr, &mRenderTargetView);
	}

	if (mTextureType == TextureType::ShaderResource) {
		MM::Get<Framework>().Context()->GenerateMips(mShaderResourceView.Get());
	}

	mPixelSize = {createDesc.width, createDesc.height};
	mStretch   = mPixelSize / 100.f;

	return true;
}

void Texture::CreateFromTexture(ComPtr<ID3D11Texture2D> texture) {
	auto& device = MM::Get<Framework>().Device();

	device->CreateShaderResourceView(texture.Get(), nullptr, &mShaderResourceView);

	if (mTextureType == TextureType::RenderTarget) {
		device->CreateRenderTargetView(texture.Get(), nullptr, &mRenderTargetView);
	}
}

void Texture::BindSRV(uint32_t slot) const {
	MM::Get<Framework>().Context()->PSSetShaderResources(slot, 1, mShaderResourceView.GetAddressOf());
}

void Texture::UnbindSRV(uint32_t slot) const {
	ID3D11ShaderResourceView* nullSRV[] = {nullptr};
	MM::Get<Framework>().Context()->PSSetShaderResources(slot, 1, nullSRV);
}

}  // namespace JR