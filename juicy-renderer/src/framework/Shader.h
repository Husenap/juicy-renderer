#pragma once

namespace JR {

class Shader {
public:
	enum ShaderType : uint8_t {
		Vertex = 1<<0,
		Geometry = 1<<1,
		Pixel = 1<<2,
	};

	bool Load(std::underlying_type_t<ShaderType> shaderType, const std::string& filepath);

	void Bind();
	void Unbind();

private:
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11GeometryShader> mGeometryShader;
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11InputLayout> mInputLayout;

	ShaderType mShaderType;
};

}  // namespace JR