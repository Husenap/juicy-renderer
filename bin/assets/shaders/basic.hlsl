#include "common.hlsl"

SamplerState DefaultSampler : register(s0);
Texture2D ColorTexture : register(t0);
Texture2D BackTexture : register(t1);

void VSMain(in VertexInput input, out GeometryInput output){
    output.position = input.position;
    output.uv = input.uv;
    output.tint = float4(input.tint.rgb, 1.0) * input.tint.a;
    output.blendMode = input.blendMode;
}

[maxvertexcount(4)]
void GSMain(point GeometryInput input[1], inout TriangleStream<PixelInput> output){
    float2 size = 0.5f;
    const float4 offset[4] = {
        { -size.x, size.y, 0, 0 },
        { size.x, size.y, 0, 0 },
        { -size.x, -size.y, 0, 0 },
        { size.x, -size.y, 0, 0 },
    };

    const float2 uv[4] = {
        {input[0].uv.x, input[0].uv.y},
        {input[0].uv.z, input[0].uv.y},
        {input[0].uv.x, input[0].uv.w},
        {input[0].uv.z, input[0].uv.w},
    };

    const float2 duv = uv[3] - uv[0];

    PixelInput vertex;
    for(int i = 0; i < 4; ++i){
        vertex.position = input[0].position;
        vertex.position.xy += offset[i].xy * duv;
        vertex.position = mul(ProjectionMatrix, vertex.position);

        vertex.uv = uv[i];
        vertex.tint = input[0].tint;
        vertex.blendMode = input[0].blendMode;

        output.Append(vertex);
    }

    output.RestartStrip();
}

void PSMain(in PixelInput input, out PixelOutput output : SV_TARGET) {
    float4 color = ColorTexture.Sample(DefaultSampler, input.uv);

    color.rgb *= color.a;

    float4 back = BackTexture.Sample(DefaultSampler, input.uv);
    back *= color.a;

    float3 frontLight = color.rgb *  float3(0.625, 0.725, 1.0).rgb * 1.0;
    float3 backLight = color.rgb * back.rgb * float3(0.325, 0.625, 1.0).bgr * 1.0;

    float3 col = frontLight + backLight;

    float4 finalColor = float4(col, color.a);
    finalColor *= input.tint;
    finalColor.a *= input.blendMode;

    output.color = finalColor;
}