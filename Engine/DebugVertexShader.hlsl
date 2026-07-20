cbuffer ConstantBuffer : register(b0)
{
    matrix wvpMat;
    float4 diffUse;
    bool isTexture;
    bool isGray;
}

struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL; // 法線
};

struct VSOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL; // 法線
};

VSOutput main(VSInput IN)
{
    VSOutput OUT;
    OUT.position = mul(float4(IN.position, 1.0f), wvpMat);
    OUT.color = diffUse;
    OUT.uv = IN.uv;
    OUT.normal = IN.normal;
    return OUT;
}