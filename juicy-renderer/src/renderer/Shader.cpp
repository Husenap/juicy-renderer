#include "Shader.h"

namespace JR {

HRESULT CompileShader(const std::string& filepath,
                      const std::string& entryPoint,
                      const std::string& profile,
                      ComPtr<ID3DBlob>& blob) {
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	std::wstring wFilepath(filepath.begin(), filepath.end());

	HRESULT hr = D3DCompileFromFile(wFilepath.c_str(),
	                                nullptr,
	                                D3D_COMPILE_STANDARD_FILE_INCLUDE,
	                                entryPoint.c_str(),
	                                profile.c_str(),
	                                flags,
	                                NULL,
	                                &shaderBlob,
	                                &errorBlob);

	if (FAILED(hr)) {
		std::cerr << "Failed to compile shader: " << filepath << std::endl;
		std::cerr << "Entry Point: " << entryPoint << std::endl;

		if (errorBlob) {
			std::cerr << static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
		}

		return hr;
	}

	blob = shaderBlob;

	return hr;
}

bool Shader::Load(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, const std::string& filepath) {
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;

	HRESULT hr = CompileShader(filepath, "VSMain", "vs_5_0", vsBlob);
	if (FAILED(hr)) {
		return false;
	}

	hr = CompileShader(filepath, "PSMain", "ps_5_0", psBlob);
	if (FAILED(hr)) {
		return false;
	}

	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &mVertexShader);
	if (FAILED(hr)) {
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}

	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &mPixelShader);
	if (FAILED(hr)) {
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	Bind(context);

	D3D11_INPUT_ELEMENT_DESC ied[] = {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}};

	hr = device->CreateInputLayout(ied, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout);
	if (FAILED(hr)) {
		std::cerr << "Failed to create the Input Layout" << std::endl;
		return false;
	}

	context->IASetInputLayout(mInputLayout.Get());

	return true;
}

void Shader::Bind(ComPtr<ID3D11DeviceContext>& context) {
	context->VSSetShader(mVertexShader.Get(), NULL, NULL);
	context->PSSetShader(mPixelShader.Get(), NULL, NULL);
}

}  // namespace JR