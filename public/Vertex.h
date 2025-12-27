#pragma once

#include <directxtk/SimpleMath.h>
#include <vector>

namespace hlab {

	using DirectX::SimpleMath::Vector2;
	using DirectX::SimpleMath::Vector3;

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texcoord;
    DirectX::SimpleMath::Vector3 tangent;
    DirectX::SimpleMath::Vector3 bitangent;
};
}
