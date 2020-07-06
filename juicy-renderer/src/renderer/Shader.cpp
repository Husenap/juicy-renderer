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

bool Shader::Load(std::underlying_type_t<ShaderType>  shaderType, const std::string& filepath) {
	mShaderType = static_cast<ShaderType>(shaderType);

	const auto compileVertexShader = [&]() {
		if (!(mShaderType & ShaderType::Vertex)) {
			return;
		}

		ComPtr<ID3DBlob> vsBlob;

		if (FAILED(CompileShader(filepath, "VSMain", "vs_5_0", vsBlob))) {
			std::cerr << "Failed to compile vertex shader!" << std::endl;
			return;
		}

		auto& device = MM::Get<Framework>().Device();
		if (FAILED(device->CreateVertexShader(
		        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &mVertexShader))) {
			std::cerr << "Failed to create vertex shader!" << std::endl;
			return;
		}

		if (FAILED(CreateInputLayout(vsBlob, mInputLayout))) {
			std::cerr << "Failed to create input layout: " << filepath << std::endl;
			return;
		}
	};

	const auto compileGeometryShader = [&]() {
		if (!(mShaderType & ShaderType::Geometry)) {
			return;
		}

		ComPtr<ID3DBlob> gsBlob;

		if (FAILED(CompileShader(filepath, "GSMain", "gs_5_0", gsBlob))) {
			std::cerr << "Failed to compile geometry shader!";
			return;
		}

		auto& device = MM::Get<Framework>().Device();
		if (FAILED(device->CreateGeometryShader(
		        gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), NULL, &mGeometryShader))) {
			std::cerr << "Failed to create geometry shader!" << std::endl;
			return;
		}
	};

	const auto compilePixelShader = [&]() {
		if (!(mShaderType & ShaderType::Pixel)) {
			return;
		}

		ComPtr<ID3DBlob> psBlob;

		if (FAILED(CompileShader(filepath, "PSMain", "ps_5_0", psBlob))) {
			std::cerr << "Failed to compile pixel shader!" << std::endl;
			return;
		}

		auto& device = MM::Get<Framework>().Device();
		if (FAILED(
		        device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &mPixelShader))) {
			std::cerr << "Failed to create pixel shader!" << std::endl;
			return;
		}

		CreateResourceBindings(psBlob);
	};

	compileVertexShader();
	compileGeometryShader();
	compilePixelShader();

	return true;
}

void Shader::Bind() {
	auto& context = MM::Get<Framework>().Context();

	if (mShaderType & ShaderType::Vertex && mVertexShader) {
		context->IASetInputLayout(mInputLayout.Get());
		context->VSSetShader(mVertexShader.Get(), NULL, NULL);
	}
	if (mShaderType & ShaderType::Geometry && mGeometryShader) {
		context->GSSetShader(mGeometryShader.Get(), NULL, NULL);
	}
	if (mShaderType & ShaderType::Pixel && mPixelShader) {
		context->PSSetShader(mPixelShader.Get(), NULL, NULL);
	}
}

void Shader::Unbind() {
	auto& context = MM::Get<Framework>().Context();

	if (mShaderType & ShaderType::Vertex && mVertexShader) {
		context->IASetInputLayout(nullptr);
		context->VSSetShader(nullptr, NULL, NULL);
	}
	if (mShaderType & ShaderType::Geometry && mGeometryShader) {
		context->GSSetShader(nullptr, NULL, NULL);
	}
	if (mShaderType & ShaderType::Pixel && mPixelShader) {
		context->PSSetShader(nullptr, NULL, NULL);
	}
}

}  // namespace JR