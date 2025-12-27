#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <iostream>

#include <windows.h>
#include <wrl.h>

namespace hlab {

	using Microsoft::WRL::ComPtr;

	struct Mesh {

        ComPtr<ID3D11Buffer> vertexBuffer;
        ComPtr<ID3D11Buffer> indexBuffer;
        ComPtr<ID3D11Buffer> vertexConstantBuffer;
        ComPtr<ID3D11Buffer> pixelConstantBuffer;

        ComPtr<ID3D11Texture2D> texture;
        ComPtr<ID3D11ShaderResourceView> textureResourceView;

        ComPtr<ID3D11Texture2D> baseColorTex;
        ComPtr<ID3D11Texture2D> normalTex;
        ComPtr<ID3D11Texture2D> ormTex;

        ComPtr<ID3D11ShaderResourceView> baseColorSRV;
        ComPtr<ID3D11ShaderResourceView> normalSRV;
        ComPtr<ID3D11ShaderResourceView> ormSRV;

        UINT m_indexCount = 0;
	};
    }
