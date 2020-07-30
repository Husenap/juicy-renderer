struct VertexInput {
    float4 position : POSITION;
    float4 uv : TEXCOORD;
    float4 tint : COLOR;
    float blendMode : BLEND_MODE;
};

struct GeometryInput{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD;
    float4 tint : COLOR;
    float blendMode : BLEND_MODE;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float2 uv : TEXCOORD;
    float4 tint : COLOR;
    float blendMode : BLEND_MODE;
};

struct PixelOutput {
    float4 color : SV_TARGET;
};

cbuffer BufferData : register(b0) {
    float4x4 ProjectionMatrix;
    float4 Resolution;
    float2 Time;
    float2 Stretch;
}

struct LightData{
    float2 position;
    float intensity;
    float size;
    float3 color;
    float isBackLight;
};
cbuffer LightBufferData : register(b1) {
    LightData Lights[64];
    int NumLights;
    float _padding[3];
}