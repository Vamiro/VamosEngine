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

    output.normal = normalize(mul(float4(input.normal, 0.0f), world).xyz);

    output.texCoord = input.texCoord;
    return output;
}

float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 texColor = tex.Sample(texSampler, input.texCoord);

    float4 lightColor = float4(1.0f, 1.0f, 0.8f, 1.0f);
    float3 lightDir = normalize(float3(0.5f, -1.0f, -0.5f));
    float3 normal = normalize(input.normal);

    float ambientStrength = 0.1f;
    float4 ambient = ambientStrength * lightColor;

    float diff = max(dot(normal, -lightDir), 0.1f);
    float4 diffuse = diff * lightColor;

    float4 result = (ambient + diffuse) * texColor * objectColor;

    return result;
}
