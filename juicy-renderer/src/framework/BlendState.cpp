#include "BlendState.h"

#include "Framework.h"

namespace JR {

bool BlendState::Create(D3D11_BLEND srcRGB,
                        D3D11_BLEND destRGB,
                        D3D11_BLEND srcAlpha,
                        D3D11_BLEND destAlpha,
                        D3D11_BLEND_OP opRGB /*= D3D11_BLEND_OP_ADD*/,
                        D3D11_BLEND_OP opAlpha /*= D3D11_BLEND_OP_ADD*/) {
	D3D11_BLEND_DESC blendDesc{
	    .RenderTarget = {D3D11_RENDER_TARGET_BLEND_DESC{.BlendEnable           = TRUE,
	                                                    .SrcBlend              = srcRGB,
	                                                    .DestBlend             = destRGB,
	                                                    .BlendOp               = opRGB,
	                                                    .SrcBlendAlpha         = srcAlpha,
	                                                    .DestBlendAlpha        = destAlpha,
	                                                    .BlendOpAlpha          = opAlpha,
	                                                    .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL}}};

	return !FAILED(MM::Get<Framework>().Device()->CreateBlendState(&blendDesc, &mBlendState));
}

void BlendState::Bind() {
	glm::vec4 blendFactor(0.f);
	MM::Get<Framework>().Context()->OMSetBlendState(mBlendState.Get(), &blendFactor.r, 0xffffffff);
}

}  // namespace JR