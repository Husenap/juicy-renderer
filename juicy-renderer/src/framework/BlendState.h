#pragma once

namespace JR {

class BlendState {
public:
	bool Create(D3D11_BLEND srcRGB,
	            D3D11_BLEND destRGB,
	            D3D11_BLEND srcAlpha,
	            D3D11_BLEND destAlpha,
	            D3D11_BLEND_OP opRGB = D3D11_BLEND_OP_ADD,
	            D3D11_BLEND_OP opAlpha = D3D11_BLEND_OP_ADD);

	void Bind();

private:
	ComPtr<ID3D11BlendState> mBlendState;
};

}  // namespace JR