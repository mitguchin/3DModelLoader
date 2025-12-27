#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxtk/SimpleMath.h>
#include <memory>

#include "Mesh.h"

namespace hlab {

	using DirectX::SimpleMath::Matrix;
	using DirectX::SimpleMath::Vector3;

	struct Material {
        Vector3 ambient;
        float shininess;
        Vector3 diffuse;
        float dummy1;
        Vector3 specular;
        float dummy2;
	};
    }