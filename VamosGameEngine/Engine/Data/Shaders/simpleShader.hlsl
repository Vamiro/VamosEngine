// Вершинный шейдер
cbuffer cbVertexShader : register(b0)
{
    matrix mat;
};


cbuffer cbPixelShader : register(b1)
{
    float4 objectColor;  // Цвет объекта
};

struct VS_IN
{
    float3 pos : POSITION;  // Позиция вершины
    float2 texCoord : TEXCOORD;  // Текстурные координаты
};

struct PS_IN
{
    float4 pos : SV_POSITION;  // Позиция после трансформации
    float2 texCoord : TEXCOORD;  // Текстурные координаты
};

Texture2D tex : register(t0);  // Текстура
SamplerState texSampler : register(s0);  // Семплер

PS_IN VSMain(VS_IN input)
{
    PS_IN output;
    output.pos = mul(float4(input.pos, 1.0f), mat);  // Трансформация позиции
    output.texCoord = input.texCoord;  // Передача текстурных координат
    return output;
}

float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 texColor = tex.Sample(texSampler, input.texCoord);

    return texColor * 1.0f - objectColor;
}