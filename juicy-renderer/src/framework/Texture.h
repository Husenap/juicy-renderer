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

	glm::vec2 GetPixelSize() const { return mStretch; }
	glm::vec2 GetStretch() const { return mStretch; }

	operator ImTextureID() const { return mShaderResourceView.Get(); }

private:
	ComPtr<ID3D11Texture2D> mTexture;
	ComPtr<ID3D11ShaderResourceView> mShaderResourceView;

	glm::vec2 mPixelSize;
	glm::vec2 mStretch;
};

}  // namespace JR