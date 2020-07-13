#pragma once

namespace JR {

class Texture {
public:
	struct TextureCreateDesc {
		uint32_t width;
		uint32_t height;
		uint8_t* data;
		DXGI_FORMAT format;
	};

	bool CreateFromFile(const std::string& filepath);
	bool Create(uint32_t w, uint32_t h, DXGI_FORMAT format, uint8_t* imageData);
	bool Create(const TextureCreateDesc& createDesc);
	void CreateFromTexture(ComPtr<ID3D11Texture2D> texture);

	void Bind(uint32_t slot) const;
	void Unbind(uint32_t slot) const;

	bool IsValid() const { return mTexture && mShaderResourceView; }

	operator ImTextureID() const { return mShaderResourceView.Get(); }

private:
	ComPtr<ID3D11Texture2D> mTexture;
	ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
};

}  // namespace JR