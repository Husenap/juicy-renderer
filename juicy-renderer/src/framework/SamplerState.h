#pragma once

namespace JR {

class SamplerState {
public:
	enum class Filter { Linear, Point, Anisotropic };

	enum class Address {
		Clamp,
		Wrap,
		Mirror,
	};

	bool Create(Filter filter = Filter::Linear, Address address = Address::Wrap);

	void Bind(uint32_t slot);
	void Unbind(uint32_t slot);

private:
	ComPtr<ID3D11SamplerState> mSamplerState;
};

}  // namespace JR