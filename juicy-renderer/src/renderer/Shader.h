#pragma once

namespace JR {

class Shader {
public:
	bool Load(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, const std::string& filepath);

	void Bind(ComPtr<ID3D11DeviceContext>& context);

private:
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11InputLayout> mInputLayout;
};

}  // namespace JR