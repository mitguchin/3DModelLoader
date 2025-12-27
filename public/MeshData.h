#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>

#include <string>
#include <vector>

#include "Vertex.h"

namespace hlab {

	using std::vector;
    using Microsoft::WRL::ComPtr;
	
	struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        std::string baseColorFilename;
        std::string normalFilename;
        std::string ormFilename;

       //std::string textureFilename;

        ComPtr<ID3D11ShaderResourceView> baseColorSRV;
        ComPtr<ID3D11ShaderResourceView> normalSRV;
        ComPtr<ID3D11ShaderResourceView> emissiveSRV;
        ComPtr<ID3D11ShaderResourceView> ormSRV;

        ComPtr<ID3D11Texture2D> baseColorTex;
        ComPtr<ID3D11Texture2D> normalTex;
        ComPtr<ID3D11Texture2D> ormTex;

	};

    }
