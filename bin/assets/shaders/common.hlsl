struct VertexInput {
    float4 position : POSITION;
};

struct GeometryInput{
    float4 position : SV_POSITION;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float2 uv : UV;
};

struct PixelOutput {
    float4 color;
};