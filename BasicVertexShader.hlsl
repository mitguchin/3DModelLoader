#include "Common.hlsli"

cbuffer BasicVertexConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, model);

    output.posWorld = pos.xyz;
    
    float4 clip = mul(pos, view);
    clip = mul(clip, projection);
    
    output.posProj = clip;
    output.texcoord = input.texcoord;
    output.color = float3(0.0f, 0.0f, 0.0f);
    
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    float4 t = float4(input.tangentModel, 0.0f);
    float4 b = float4(input.bitangentModel, 0.0f);
    
    output.tangentWorld = normalize(mul(t, invTranspose).xyz);
    output.bitangentWorld = normalize(mul(b, invTranspose).xyz);
    
    return output;
}