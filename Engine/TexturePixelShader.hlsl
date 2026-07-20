Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    matrix wvpMat; // ワールド・ビュー・プロジェクション行列
    float4 diffUse; // ディフューズ（マテリアル自体の色）
    float4 ambient; // アンビエントカラー（影の色）
    float4 speculer; // スペキュラー（反射した時の色）
    int isTexture; // テクスチャが貼ってあるからどうか（1: 貼ってある、0: 貼ってない）
    int isGray; // グレースケールが有効かどうか（1: 有効、0: 無効）
    int isMosaic; // モザイクが有効かどうか（1：有効、0：無効）
}

struct PSInput
{
    float4 position : SV_Position; // 頂点の場所
    float4 color : COLOR; // 頂点の色
    float2 uv : TEXCOORD0; //UV座標
    float3 normal : NORMAL; // 法線
};

float4 main(PSInput IN) : SV_Target
{
    float4 diffUse_ = { 1.0, 0.0, 0.0, 1.0 };
    if (isTexture)
    {
        diffUse_ = texture0.Sample(sampler0, IN.uv); //テクスチャがある場合は、テクスチャの色にする
    }
    else
    {
        diffUse_ = diffUse; // テクスチャがない場合は、マテリアル自体の色
    }

    if (isGray)
    { //グレースケールが有効な場合の処理
        float r = 0.299 * diffUse_.r;
        float g = 0.587 * diffUse_.g;
        float b = 0.114 * diffUse_.b;
        diffUse_ = r + g + b;
    }
    
    if (isMosaic)
    {
        float mosaicSize = 0.2; // 0.0~1.0の範囲（※小さければモザイクが大きくなる）
        float2 posterize = floor(IN.uv * (mosaicSize * 100) + 0.5) / (mosaicSize * 100);
        diffUse_ = texture0.Sample(sampler0, posterize);
    }

    return diffUse_ + diffUse_ * ambient + speculer;
}