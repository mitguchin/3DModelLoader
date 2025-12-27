#include "Common.hlsli"

cbuffer BasicVertexConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

cbuffer NormalVertexConstantBuffer : register(b1)
{
    float scale;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output = (PixelShaderInput) 0;

    float4 pos = float4(input.posModel, 1.0f);

    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = normalize(mul(normal, invTranspose).xyz);
    
    float4 t = float4(input.tangentModel, 0.0f);
    float4 b = float4(input.bitangentModel, 0.0f);
    output.tangentWorld = normalize(mul(t, invTranspose).xyz);
    output.bitangentWorld = normalize(mul(b, invTranspose).xyz);

    pos = mul(pos, model);
    
    float u = input.texcoord.x;

    pos.xyz += output.normalWorld * u * scale;

    output.posWorld = pos.xyz;

    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.posProj = pos;
    output.texcoord = input.texcoord;

    output.color = float3(1.0, 1.0, 0.0) * (1.0 - u) + float3(1.0, 0.0, 0.0) * u;
    return output;

}