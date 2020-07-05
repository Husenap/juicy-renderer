#include "common.hlsl"

PixelInput VSMain(VertexInput input){
    PixelInput output;

    output.position = input.position;
    output.uv = input.uv;

    return output;
}

PixelOutput PSMain(PixelInput input) : SV_TARGET {
    PixelOutput output;

    float2 uv = input.uv;
    uv = uv*2.0f-1.0f;

    float f = length(uv);

    f = smoothstep(0.29, 0.3, f);

    output.color = float4(f, f, f, f);

    return output;
}