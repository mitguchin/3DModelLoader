#include "ExampleApp.h"

#include <fstream> 
#include <filesystem>
#include <cstddef>
#include <tuple>
#include <vector>

#include "GeometryGenerator.h"

namespace hlab {

	using namespace std;

	ExampleApp::ExampleApp() : AppBase(), m_BasicPixelConstantBufferData() {}

    static void Create1x1TextureSRV(ID3D11Device *device, uint8_t r, uint8_t g,
                                    uint8_t b, uint8_t a, bool srgb,
                                    ComPtr<ID3D11Texture2D> &outTex,
                                    ComPtr<ID3D11ShaderResourceView> &outSRV) {
        uint32_t pixel = (uint32_t(r)) | (uint32_t(g) << 8) |
                         (uint32_t(b) << 16) | (uint32_t(a) << 24);

        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = 1;
        desc.Height = 1;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format =
            srgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA init{};
        init.pSysMem = &pixel;
        init.SysMemPitch = 4;

        device->CreateTexture2D(&desc, &init, outTex.ReleaseAndGetAddressOf());
        device->CreateShaderResourceView(outTex.Get(), nullptr,
                                         outSRV.ReleaseAndGetAddressOf());
    }


	bool ExampleApp::Initialize() {
	
		if (!AppBase::Initialize())
            return false;

		D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        Create1x1TextureSRV(m_device.Get(), 255, 255, 255, 255, true,
                            m_defaultWhiteTex, m_defaultWhiteSRV);

        Create1x1TextureSRV(m_device.Get(), 128, 128, 255, 255, false,
                            m_defaultNormalTex, m_defaultNormalSRV);

        Create1x1TextureSRV(m_device.Get(), 255, 255, 0, 255, false,
                            m_defaultOrmTex, m_defaultOrmSRV);

        m_device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

        auto meshes = GeometryGenerator::ReadFromFile(
            "C:\\Temp\\Shield\\", "shield_l.fbx");

        m_BasicVertexConstantBufferData.model = Matrix();
        m_BasicVertexConstantBufferData.view = Matrix();
        m_BasicVertexConstantBufferData.projection = Matrix();
        ComPtr<ID3D11Buffer> vertexConstantBuffer;
        ComPtr<ID3D11Buffer> pixelConstantBuffer;
        AppBase::CreateConstantBuffer(m_BasicVertexConstantBufferData, 
            vertexConstantBuffer);
        AppBase::CreateConstantBuffer(m_BasicPixelConstantBufferData,
                                      pixelConstantBuffer);

        for (const auto &meshData : meshes) {
            auto newMesh = std::make_shared<Mesh>();
            AppBase::CreateVertexBuffer(meshData.vertices,
                                        newMesh->vertexBuffer);
            newMesh->m_indexCount = UINT(meshData.indices.size());
            AppBase::CreateIndexBuffer(meshData.indices, newMesh->indexBuffer);

            if (!meshData.baseColorFilename.empty()) {
                AppBase::CreateTexture(meshData.baseColorFilename,
                                       newMesh->baseColorTex,
                                       newMesh->baseColorSRV, true);
            }

            if (!meshData.normalFilename.empty()) {
                AppBase::CreateTexture(meshData.normalFilename,
                                       newMesh->normalTex, newMesh->normalSRV, false);
            }

      std::string ormToUse = meshData.ormFilename;

            if (ormToUse.empty() && !meshData.baseColorFilename.empty()) 
            {
                ormToUse = meshData.baseColorFilename;

                auto pos = ormToUse.find("BaseColor");
                if (pos != std::string::npos)
                    ormToUse.replace(pos, strlen("BaseColor"), "ORM");
            }

            if (!ormToUse.empty() && std::ifstream(ormToUse).good()) {
                AppBase::CreateTexture(ormToUse, newMesh->ormTex,
                                       newMesh->ormSRV, false);
            }

            newMesh->vertexConstantBuffer = vertexConstantBuffer;
            newMesh->pixelConstantBuffer = pixelConstantBuffer;

            this->m_meshes.push_back(newMesh);

            printf("[Mesh] BC=%s\n", meshData.baseColorFilename.c_str());
            printf("[Mesh] N =%s\n", meshData.normalFilename.c_str());
            printf("[Mesh] ORM=%s\n", ormToUse.c_str());
        }

        vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, (UINT)offsetof(Vertex,position),
             D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
             (UINT)offsetof(Vertex, normal),
             D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
             (UINT)offsetof(Vertex, texcoord),
             D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
             (UINT)offsetof(Vertex, tangent), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
             (UINT)offsetof(Vertex, bitangent), D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        AppBase::CreateVertexShaderAndInputLayout(
            L"BasicVertexShader.hlsl", basicInputElements, m_basicVertexShader,
            m_basicInputLayout);

        AppBase::CreatePixelShader(L"BasicPixelShader.hlsl",
                                   m_basicPixelShader);

        m_normalLines = std::make_shared<Mesh>();

        std::vector<Vertex> normalVertices;
        std::vector<uint32_t> normalIndices;

        size_t offset = 0;
        for (const auto &meshData : meshes) {
            for (size_t i = 0; i < meshData.vertices.size(); i++) {
                
                auto v = meshData.vertices[i];

                v.texcoord.x = 0.0f;
                normalVertices.push_back(v);

                v.texcoord.x = 1.0f;
                normalVertices.push_back(v);

                normalIndices.push_back(uint32_t(2 * (i + offset)));
                normalIndices.push_back(uint32_t(2 * (i + offset) + 1));
            }
            offset += meshData.vertices.size();
        }

        AppBase::CreateVertexBuffer(normalVertices,
                                    m_normalLines->vertexBuffer);
        m_normalLines->m_indexCount = UINT(normalIndices.size());
        AppBase::CreateIndexBuffer(normalIndices, m_normalLines->indexBuffer);
        AppBase::CreateConstantBuffer(m_normalVertexConstantBufferData,
                                      m_normalLines->vertexConstantBuffer);
        AppBase::CreateVertexShaderAndInputLayout(
            L"NormalVertexShader.hlsl", basicInputElements, m_normalVertexShader,
            m_basicInputLayout);
        AppBase::CreatePixelShader(L"NormalPixelShader.hlsl", m_normalPixelShader);                                        
        return true;
    }

    void ExampleApp::Update(float dt) {
        
        using namespace DirectX;

        m_BasicVertexConstantBufferData.model =
            Matrix::CreateScale(m_modelScaling) *
            Matrix::CreateRotationX(m_modelRotation.x) *
            Matrix::CreateRotationY(m_modelRotation.y) *
            Matrix::CreateRotationZ(m_modelRotation.z) *
            Matrix::CreateTranslation(m_modelTranslation);
        m_BasicVertexConstantBufferData.model =
            m_BasicVertexConstantBufferData.model.Transpose();

         m_BasicVertexConstantBufferData.invTranspose =
            m_BasicVertexConstantBufferData.model;
        m_BasicVertexConstantBufferData.invTranspose.Translation(Vector3(0.0f));
        m_BasicVertexConstantBufferData.invTranspose =
            m_BasicVertexConstantBufferData.invTranspose.Transpose().Invert();

        m_BasicVertexConstantBufferData.view =
            Matrix::CreateRotationY(m_viewRot) *
            Matrix::CreateTranslation(0.0f, 0.0f, 2.0f);

         m_BasicPixelConstantBufferData.eyeWorld = Vector3::Transform(
            Vector3(0.0f), m_BasicVertexConstantBufferData.view.Invert());

         m_BasicVertexConstantBufferData.view =
             m_BasicVertexConstantBufferData.view.Transpose();

         const float aspect = AppBase::GetAspectRatio();
         if (m_usePerspectiveProjection) {
             m_BasicVertexConstantBufferData.projection = XMMatrixPerspectiveFovLH( 
                     XMConvertToRadians(m_projFovAngleY), aspect, m_nearZ, m_farZ);
         } else {
             m_BasicVertexConstantBufferData.projection =
                 XMMatrixOrthographicOffCenterLH(-aspect, aspect, -1.0f, 1.0f,
                                                 m_nearZ, m_farZ);
         }
         m_BasicVertexConstantBufferData.projection =
             m_BasicVertexConstantBufferData.projection.Transpose();

       for (auto &mesh : m_meshes) {
             if (mesh) {
                 AppBase::UpdateBuffer(m_BasicVertexConstantBufferData,
                                       mesh->vertexConstantBuffer);
             }
         }

         m_BasicPixelConstantBufferData.material.diffuse =
             Vector3(m_materialDiffuse);
         m_BasicPixelConstantBufferData.material.specular =
             Vector3(m_materialSpecular);

        for (int i = 0; i < MAX_LIGHTS; i++) {
             if (i != m_lightType) {
                 m_BasicPixelConstantBufferData.lights[i].strength =
                     Vector3(0.0f, 0.0f, 0.0f);
             } else {
                 m_BasicPixelConstantBufferData.lights[i] = m_lightFromGUI;
                 m_BasicPixelConstantBufferData.lights[i].strength =
                     Vector3(1.0f, 1.0f, 1.0f);
             }
         }

        for (auto &mesh : m_meshes) {
             if (mesh) {
                 AppBase::UpdateBuffer(m_BasicPixelConstantBufferData,
                                       mesh->pixelConstantBuffer);
             }
         }

         if (m_drawNormals && m_drawNormalsDirtyFlag) {
             AppBase::UpdateBuffer(m_normalVertexConstantBufferData,
                                   m_normalLines->vertexConstantBuffer);

             m_drawNormalsDirtyFlag = false;
         }
    }

    void ExampleApp::Render() {
        
        SetViewport();

        float clearColor[4] = {
            0.0f, 0.0f, 0.0f, 1.0f
        };
        m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
        m_context->ClearDepthStencilView(m_depthStencilView.Get(), 
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.0f, 0);
        m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(),
        m_depthStencilView.Get());
        m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
        m_context->VSSetShader(m_basicVertexShader.Get(), 0, 0);
        m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf()); 
        m_context->PSSetShader(m_basicPixelShader.Get(), 0, 0);

        if (m_drawAsWire) {
            m_context->RSSetState(m_wireRasterizerState.Get());
        } else {
            m_context->RSSetState(m_solidRasterizerState.Get());
        }

        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        for (const auto &mesh : m_meshes) {
            m_context->VSSetConstantBuffers(
                0, 1, mesh->vertexConstantBuffer.GetAddressOf());

            auto bc = mesh->
                baseColorSRV ?
                mesh->baseColorSRV.Get()
                                         : m_defaultWhiteSRV.Get();

            auto nor = mesh->
                normalSRV ? mesh->normalSRV.Get()
                                       : m_defaultNormalSRV.Get();

            auto orm =
                mesh->ormSRV ? 
                mesh->ormSRV.Get() : m_defaultOrmSRV.Get();

            ID3D11ShaderResourceView *srvs[3] = {bc, nor, orm};
            m_context->PSSetShaderResources(0, 3, srvs);

            m_context->PSSetConstantBuffers(
                0, 1, mesh->pixelConstantBuffer.GetAddressOf());

            m_context->IASetInputLayout(m_basicInputLayout.Get());
            m_context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), 
            &stride, &offset);
            m_context->IASetIndexBuffer(mesh->indexBuffer.Get(),
                                        DXGI_FORMAT_R32_UINT, 0);
            m_context->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_context->DrawIndexed(mesh->m_indexCount, 0, 0);
        }

        if (m_drawNormals) {
            m_context->VSSetShader(m_normalVertexShader.Get(), 0, 0);

            ID3D11Buffer *pptr[2] = {m_meshes[0]->vertexConstantBuffer.Get(),
                                     m_normalLines->vertexConstantBuffer.Get()};
            m_context->VSSetConstantBuffers(0, 2, pptr);
            m_context->PSSetShader(m_normalPixelShader.Get(), 0, 0);

            m_context->IASetVertexBuffers(
                0, 1, m_normalLines->vertexBuffer.GetAddressOf(), &stride, &offset);
            m_context->IASetIndexBuffer(m_normalLines->indexBuffer.Get(),
                DXGI_FORMAT_R32_UINT, 0);
            m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            m_context->DrawIndexed(m_normalLines->m_indexCount, 0, 0);
        }
    }

    void ExampleApp::UpdateGUI() {
        bool useTex = (m_BasicPixelConstantBufferData.useTexture != 0);
        if (ImGui::Checkbox("Use Texture", &useTex))
        {
            m_BasicPixelConstantBufferData.useTexture = useTex ? 1 : 0;
        }
        ImGui::Checkbox("Wireframe", &m_drawAsWire);
        ImGui::Checkbox("Draw Normals", &m_drawNormals);
        if (ImGui::SliderFloat("Normal scale",
                               &m_normalVertexConstantBufferData.scale, 0.0f,
                               1.0f)) {
            m_drawNormalsDirtyFlag = true;
        }
        ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
        ImGui::SliderFloat3("m_modelRotation", &m_modelRotation.x, -3.14f, 3.14f);
        ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);
        ImGui::SliderFloat("Material Shininess", 
            &m_BasicPixelConstantBufferData.material.shininess, 1.0f,
                            256.0f);
        if (ImGui::RadioButton("Directional Light", m_lightType == 0)) {
            m_lightType = 0;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Point Light", m_lightType == 1)) {
            m_lightType = 1;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Spot Light", m_lightType == 2)) {
            m_lightType = 2;
        }
        ImGui::SliderFloat("Material Diffuse", &m_materialDiffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("Material Specular", &m_materialSpecular, 0.0f, 1.0f);
        ImGui::SliderFloat3("Light Position", &m_lightFromGUI.position.x, -5.0f, 5.0f);
        ImGui::SliderFloat("Light fallOffStart", &m_lightFromGUI.fallOffStart,
                           0.0f, 5.0f);
        ImGui::SliderFloat("Light fallOffEnd", &m_lightFromGUI.fallOffEnd,
                           0.0f, 10.0f);
        ImGui::SliderFloat("Light spotPower", &m_lightFromGUI.spotPower,
                           1.0f, 512.0f);
    }

   
 }