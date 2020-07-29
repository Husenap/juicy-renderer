#include "ShaderReflection.h"

namespace JR {

bool ShaderReflection::Reflect(ComPtr<ID3DBlob> blob) {
	HRESULT hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&mReflection));
	if (FAILED(hr)) {
		LOG_ERROR("Failed to get Shader Reflection Interface");
		return false;
	}

	mReflection->GetDesc(&mShaderDesc);

	return true;
}

void ShaderReflection::ProcessInputParameters(std::function<void(D3D11_SIGNATURE_PARAMETER_DESC)> visitor) {
	for (UINT i = 0; i < mShaderDesc.InputParameters; ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		mReflection->GetInputParameterDesc(i, &paramDesc);

		visitor(paramDesc);
	}
}

void ShaderReflection::ProcessBoundResources(std::function<void(D3D11_SHADER_INPUT_BIND_DESC)> visitor) {
	for (UINT i = 0; i < mShaderDesc.BoundResources; ++i) {
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		mReflection->GetResourceBindingDesc(i, &resourceDesc);

		visitor(resourceDesc);
	}
}

}  // namespace JR