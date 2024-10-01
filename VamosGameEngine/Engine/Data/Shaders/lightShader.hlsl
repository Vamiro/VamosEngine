cbuffer cbVertexShader : register(b0)
{
    matrix worldViewProj;
    matrix invTransWorld;
};

cbuffer cbPixelShader : register(b1)
{
    float4 objectColor;
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
    PS_IN output;
    output.pos = mul(float4(input.pos, 1.0f), worldViewProj);
    output.normal = normalize(mul(float4(input.normal, 0.0f), invTransWorld).xyz);
    output.texCoord = input.texCoord;
    return output;
}

float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 texColor = tex.Sample(texSampler, input.texCoord);

    float3 lightDir = normalize(float3(0.0f, -1.0f, -0.5f));
    float3 normal = normalize(input.normal);

    float diff = max(dot(normal, -lightDir), 0.0f);

    return texColor * objectColor * diff;
}
