#include "common.hlsl"

SamplerState DefaultSampler : register(s0);
Texture2D ColorTexture : register(t0);
Texture2D BackTexture : register(t1);

void VSMain(in VertexInput input, out GeometryInput output){
    output.position = input.position;
}

[maxvertexcount(4)]
void GSMain(point GeometryInput input[1], inout TriangleStream<PixelInput> output){
    float2 size = 0.9f;
    const float4 offset[4] = {
        { -size.x, size.y, 0, 0 },
        { size.x, size.y, 0, 0 },
        { -size.x, -size.y, 0, 0 },
        { size.x, -size.y, 0, 0 },
    };

    const float2 uv[4] = {
        {0, 0},
        {1, 0},
        {0, 1},
        {1, 1},
    };

    PixelInput vertex;
    for(int i = 0; i < 4; ++i){
        vertex.position = input[0].position + offset[i];
        vertex.uv = uv[i];

        output.Append(vertex);
    }

    output.RestartStrip();
}

void PSMain(in PixelInput input, out PixelOutput output : SV_TARGET) {
    float4 color = ColorTexture.Sample(DefaultSampler, input.uv);
    color.rgb *= color.a;
    float4 back = BackTexture.Sample(DefaultSampler, input.uv);
    back *= color.a;

    output.color = lerp(color, back, smoothstep(0.49, 0.51, input.uv.x));
}