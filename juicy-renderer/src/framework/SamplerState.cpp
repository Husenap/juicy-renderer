#include "SamplerState.h"

#include "Framework.h"

namespace JR {

bool SamplerState::Create(Filter filter, Address address) {
	static constexpr glm::vec4 border(0.f);

	D3D11_FILTER descFilter;
	switch (filter) {
	case Filter::Linear:
		descFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case Filter::Point:
		descFilter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case Filter::Anisotropic:
		descFilter = D3D11_FILTER_ANISOTROPIC;
		break;
	default:
		descFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	}

	D3D11_TEXTURE_ADDRESS_MODE descAddress;
	switch (address) {
	case Address::Clamp:
		descAddress = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case Address::Wrap:
		descAddress = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case Address::Mirror:
		descAddress = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	default:
		descAddress = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	}

	CD3D11_SAMPLER_DESC samplerDesc(descFilter,
	                                descAddress,
	                                descAddress,
	                                descAddress,
	                                0.f,
	                                16,
	                                D3D11_COMPARISON_ALWAYS,
	                                &border.x,
	                                0,
	                                D3D11_FLOAT32_MAX);

	return !FAILED(MM::Get<Framework>().Device()->CreateSamplerState(&samplerDesc, &mSamplerState));
}

void SamplerState::Bind(uint32_t slot) {
	MM::Get<Framework>().Context()->PSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
}

void SamplerState::Unbind(uint32_t slot) {
	ID3D11SamplerState* nullSampler = nullptr;
	MM::Get<Framework>().Context()->PSSetSamplers(slot, 1, &nullSampler);
}

}  // namespace JR
