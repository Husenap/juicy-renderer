#include "Shader.h"

#include "Framework.h"

#include "ShaderReflection.h"

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

DXGI_FORMAT FormatFromComponentType(BYTE mask, D3D_REGISTER_COMPONENT_TYPE componentType) {
	if (mask == 1) {
		switch (componentType) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32_FLOAT;
		}
	} else if (mask <= 3) {
		switch (componentType) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32_FLOAT;
		}
	} else if (mask <= 7) {
		switch (componentType) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32B32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		}
	} else if (mask <= 15) {
		switch (componentType) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32B32A32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}

HRESULT CreateInputLayout(ComPtr<ID3DBlob> vsBlob, ComPtr<ID3D11InputLayout>& inputLayout) {
	ShaderReflection shaderReflection;
	shaderReflection.Reflect(vsBlob);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;

	shaderReflection.ProcessInputParameters([&](auto paramDesc) {
		auto elementDesc = D3D11_INPUT_ELEMENT_DESC{
		    .SemanticName      = paramDesc.SemanticName,
		    .SemanticIndex     = paramDesc.SemanticIndex,
		    .Format            = FormatFromComponentType(paramDesc.Mask, paramDesc.ComponentType),
		    .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
		};

		inputElementDescs.push_back(elementDesc);
	});

	HRESULT hr = MM::Get<Framework>().Device()->CreateInputLayout(inputElementDescs.data(),
	                                                              inputElementDescs.size(),
	                                                              vsBlob->GetBufferPointer(),
	                                                              vsBlob->GetBufferSize(),
	                                                              &inputLayout);
	if (FAILED(hr)) {
		std::cerr << "Failed to create the Input Layout" << std::endl;
		return hr;
	}

	return hr;
}

void CreateResourceBindings(ComPtr<ID3DBlob> psBlob) {
	ShaderReflection shaderReflection;
	shaderReflection.Reflect(psBlob);

	shaderReflection.ProcessBoundResources(
	    [](auto resourceDesc) { std::cout << "resource: " << resourceDesc.Name << std::endl; });
}

bool Shader::Load(const std::string& filepath) {
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> gsBlob;
	ComPtr<ID3DBlob> psBlob;

	auto& device = MM::Get<Framework>().Device();

	HRESULT hr = CompileShader(filepath, "VSMain", "vs_5_0", vsBlob);
	if (FAILED(hr)) {
		return false;
	}

	hr = CompileShader(filepath, "GSMain", "gs_5_0", gsBlob);
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

	hr = CreateInputLayout(vsBlob, mInputLayout);
	if (FAILED(hr)) {
		std::cerr << "Failed to create input layout: " << filepath << std::endl;
		return false;
	}

	hr = device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), NULL, &mGeometryShader);
	if (FAILED(hr)) {
		std::cerr << "Failed to create geometry shader!" << std::endl;
		return false;
	}

	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &mPixelShader);
	if (FAILED(hr)) {
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	CreateResourceBindings(psBlob);

	return true;
}

void Shader::Bind() {
	auto& context = MM::Get<Framework>().Context();

	if (mVertexShader) {
		context->IASetInputLayout(mInputLayout.Get());
		context->VSSetShader(mVertexShader.Get(), NULL, NULL);
	}
	if (mGeometryShader) {
		context->GSSetShader(mGeometryShader.Get(), NULL, NULL);
	}
	if (mPixelShader) {
		context->PSSetShader(mPixelShader.Get(), NULL, NULL);
	}
}

void Shader::Unbind() {
	auto& context = MM::Get<Framework>().Context();

	if (mVertexShader) {
		context->IASetInputLayout(nullptr);
		context->VSSetShader(nullptr, NULL, NULL);
	}
	if (mGeometryShader) {
		context->GSSetShader(nullptr, NULL, NULL);
	}
	if (mPixelShader) {
		context->PSSetShader(nullptr, NULL, NULL);
	}
}

}  // namespace JR