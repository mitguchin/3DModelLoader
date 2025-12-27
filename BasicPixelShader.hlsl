#include "Common.hlsli"

Texture2D baseColorTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D roughnessTex : register(t2);
Texture2D metallicTex : register(t3);

SamplerState samp : register(s0);

cbuffer BasicPixelConstantBuffer : register(b0)
{
    float3 eyeWorld;
    int useTexture;
    Material material;
    Light light[MAX_LIGHTS];
};

static float3 DecodeNormalDX(float3 n)
{
    return normalize(n * 2.0f - 1.0f);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 toEye = normalize(eyeWorld - input.posWorld);
    float3 N = normalize(input.normalWorld);
    
    float3 baseColor = material.diffuse;
    float roughness = 0.5f;
    float metallic = 0.0f;

    if (useTexture != 0) 
    {

        baseColor = baseColorTex.Sample(samp, input.texcoord).rgb;

        float3 normalSample = normalTex.Sample(samp, input.texcoord).rgb;

        float3 nTS = DecodeNormalDX(normalSample);
        float3x3 TBN = float3x3(normalize(input.tangentWorld), normalize(input.bitangentWorld), N);
        N = normalize(mul(nTS, TBN));

        roughness = roughnessTex.Sample(samp, input.texcoord).r;
        metallic = metallicTex.Sample(samp, input.texcoord).r;
    }

    float shininess = lerp(256.0f, 2.0f, roughness);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    float3 specColor = lerp(F0, baseColor, metallic);
    float3 diffColor = baseColor * (1.0f - metallic);

    Material mat = material;
    mat.diffuse = diffColor;
    mat.specular = specColor;
    mat.shininess = shininess;

    float3 color = 0.0f;
    color += ComputeDirectionalLight(light[0], mat, N, toEye);
    color += ComputePointLight(light[1], mat, input.posWorld, N, toEye);
    color += ComputeSpotLight(light[2], mat, input.posWorld, N, toEye);

    return float4(pow(max(color, 0.0f), 1.0 / 2.2), 1.0f);
}