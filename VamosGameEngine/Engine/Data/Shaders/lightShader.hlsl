cbuffer cbVertexShader : register(b0)
{
    float4x4 world;
    float4x4 cameraView;
    float4x4 cameraProj;
    float4x4 InvWorldView;
};

cbuffer cbPixelShader : register(b1)
{
    float4 objectColor;
    float3 lightDir;
};

struct VS_IN
{
    float3 pos : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.pos = mul(float4(input.pos, 1.0f), world);
    output.pos = mul(output.pos, cameraView);
    output.pos = mul(output.pos, cameraProj);

    output.normal = mul(float4(input.normal, 0.0f), InvWorldView);

    output.texCoord = input.texCoord;
    return output;
}

float4 PSMain(PS_IN input) : SV_TARGET
{
    float3 mNormal = normalize(input.normal);
    float3 mLightColor = float3(1.0f, 1.0f, 0.95f);
    float3 mLightDir = normalize(-lightDir);

    float4 mTexColor = tex.Sample(texSampler, input.texCoord);
    float3 diffuse = max(0, dot(mLightDir, mNormal)) * mTexColor;

    float ambientStrength = 0.1f;
    float3 ambient = ambientStrength * mTexColor * objectColor;

    float4 result = float4(ambient + diffuse * mLightColor,1.0f) * objectColor * mTexColor;

    return result;
}
