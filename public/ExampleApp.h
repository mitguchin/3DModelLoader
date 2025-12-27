#pragma once

#include <algorithm>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "GeometryGenerator.h"
#include "Material.h"

namespace hlab {

	using DirectX::SimpleMath::Matrix;
	using DirectX::SimpleMath::Vector2;
	using DirectX::SimpleMath::Vector3;
	using DirectX::SimpleMath::Vector4;

	struct Light {
        Vector3 strength = Vector3(1.0f);
        float fallOffStart = 0.0f;
        Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
        float fallOffEnd = 10.0f;
        Vector3 position = Vector3(0.0f, 0.0f, -2.0f);
        float spotPower = 1.0f;
	};

    struct BasicVertexConstantBuffer {
        Matrix model;
        Matrix invTranspose;
        Matrix view;
        Matrix projection;
    };

    static_assert((sizeof(BasicVertexConstantBuffer) % 16) == 0, 
        "Constant Buffer size is 16-byte aligned");
    
    #define MAX_LIGHTS 3

    struct BasicPixelConstantBuffer {
        Vector3 eyeWorld;
        int useTexture;
        Material material;
        Light lights[MAX_LIGHTS];
    };

     static_assert((sizeof(BasicPixelConstantBuffer) % 16) == 0,
                  "Constant Buffer size is 16-byte aligned");

     struct NormalVertexConstantBuffer {
         float scale = 0.1f;
         float dummy[3];
     };

     class ExampleApp : public AppBase {
       public:
         ExampleApp();

         virtual bool Initialize() override;
         virtual void UpdateGUI() override;
         virtual void Update(float dt) override;
         virtual void Render() override;

         protected:
         ComPtr<ID3D11VertexShader> m_basicVertexShader;
         ComPtr<ID3D11PixelShader> m_basicPixelShader;
         ComPtr<ID3D11InputLayout> m_basicInputLayout;

         std::vector<shared_ptr<Mesh>> m_meshes;

         ComPtr<ID3D11SamplerState> m_samplerState;

         BasicVertexConstantBuffer m_BasicVertexConstantBufferData;
         BasicPixelConstantBuffer m_BasicPixelConstantBufferData;

         bool m_usePerspectiveProjection = true;
         Vector3 m_modelTranslation = Vector3(0.0f);
         Vector3 m_modelRotation = Vector3(-0.286f, 0.058f, 0.0f);
         Vector3 m_modelScaling = Vector3(1.8f);
         float m_viewRot = 0.0f;

         float m_projFovAngleY = 70.0f;
         float m_nearZ = 0.01f;
         float m_farZ = 100.0f;

         int m_lightType = 0;
         Light m_lightFromGUI;
         float m_materialDiffuse = 0.8f;
         float m_materialSpecular = 1.0f;

         ComPtr<ID3D11VertexShader> m_normalVertexShader;
         ComPtr<ID3D11PixelShader> m_normalPixelShader;

         shared_ptr<Mesh> m_normalLines;
         NormalVertexConstantBuffer m_normalVertexConstantBufferData;

         ComPtr<ID3D11Texture2D> m_defaultWhiteTex;
         ComPtr<ID3D11Texture2D> m_defaultNormalTex;
         ComPtr<ID3D11Texture2D> m_defaultOrmTex;

         ComPtr<ID3D11ShaderResourceView> m_defaultWhiteSRV;
         ComPtr<ID3D11ShaderResourceView> m_defaultNormalSRV;
         ComPtr<ID3D11ShaderResourceView> m_defaultOrmSRV;

         bool m_drawNormals = false;
         bool m_drawNormalsDirtyFlag = false;
     };
    }
