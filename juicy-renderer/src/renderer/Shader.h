#pragma once

namespace JR {

class Shader {
public:
	bool Load(const std::string& filepath);

	void Bind();
	void Unbind();

private:
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11GeometryShader> mGeometryShader;
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11InputLayout> mInputLayout;
};

}  // namespace JR