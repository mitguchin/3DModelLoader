#include "GeometryGenerator.h"

#include "ModelLoader.h"

namespace hlab {
	
	using namespace std;
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	MeshData GeometryGenerator::MakeSquare() { 
		vector<Vector3> positions;
        vector<Vector3> colors;
        vector<Vector3> normals;
        vector<Vector2> texcoords;

		const float scale = 1.0f;

		positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
        positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
        positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
        positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
        colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
        colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
        colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
        colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

		texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 1.0f));
        texcoords.push_back(Vector2(0.0f, 1.0f));

        MeshData meshData;

        for (size_t i = 0; i < positions.size(); i++) {
            Vertex v;
            v.position = positions[i];
            v.normal = normals[i];
            v.texcoord = texcoords[i];

            meshData.vertices.push_back(v);
        }
        meshData.indices = {
            0,1,2,0,2,3
        };

        return meshData;
	}

    MeshData GeometryGenerator::MakeBox() {

        vector<Vector3> positions;
        vector<Vector3> colors;
        vector<Vector3> normals;
        vector<Vector2> texcoords;

        const float scale = 1.0f;

        //Up
        positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
        positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
        positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
        positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);

        colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
        colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
        colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
        colors.push_back(Vector3(1.0f, 0.0f, 0.0f));

        normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
        normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
        normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
        normals.push_back(Vector3(0.0f, 1.0f, 0.0f));

        texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 1.0f));
        texcoords.push_back(Vector2(0.0f, 1.0f));

        // Down
        positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
        positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
        positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
        positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);

        colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
        colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
        colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
        colors.push_back(Vector3(0.0f, 1.0f, 0.0f));

        normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
        normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
        normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
        normals.push_back(Vector3(0.0f, -1.0f, 0.0f));

        texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 1.0f));
        texcoords.push_back(Vector2(0.0f, 1.0f));

         // Front
        positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
        positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
        positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
        positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);

        colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
        colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
        colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
        colors.push_back(Vector3(0.0f, 0.0f, 1.0f));

        normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

        texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 1.0f));
        texcoords.push_back(Vector2(0.0f, 1.0f));

        // Back
        positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
        positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
        positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
        positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);

        colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
        colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
        colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
        colors.push_back(Vector3(0.0f, 1.0f, 1.0f));

        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));

        texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 1.0f));
        texcoords.push_back(Vector2(0.0f, 1.0f));

        // Left
        positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
        positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
        positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
        positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);

        colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
        colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
        colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
        colors.push_back(Vector3(1.0f, 1.0f, 0.0f));

        normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
        normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
        normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
        normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));

        texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 1.0f));
        texcoords.push_back(Vector2(0.0f, 1.0f));

        // Right
        positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
        positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
        positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
        positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);

        colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
        colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
        colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
        colors.push_back(Vector3(1.0f, 0.0f, 1.0f));

        normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
        normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
        normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
        normals.push_back(Vector3(1.0f, 0.0f, 0.0f));

        texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 1.0f));
        texcoords.push_back(Vector2(0.0f, 1.0f));

        MeshData meshData;
        for (size_t i = 0; i < positions.size(); i++) {
            Vertex v;
            v.position = positions[i];
            v.normal = normals[i];
            v.texcoord = texcoords[i];
            meshData.vertices.push_back(v);
        }

        meshData.indices = {
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23
        };

        return meshData;
    }

    MeshData GeometryGenerator::MakeCylinder(const float bottomRadius,
        const float topRadius, float height, 
        int numSlices) {

        const float dTheta = -XM_2PI / float(numSlices);

        MeshData meshData;

        vector<Vertex> &vertices = meshData.vertices;

        for (int i = 0; i <= numSlices; i++) {
            Vertex v;
            v.position =
                Vector3::Transform(Vector3(bottomRadius, -0.5f * height, 0.0f),
                                   Matrix::CreateRotationY(dTheta * float(i)));

            v.normal = v.position - Vector3(0.0f, -0.5f * height, 0.0f);
            v.normal.Normalize();
            v.texcoord = Vector2(float(i) / numSlices, 1.0f);

            vertices.push_back(v);
        }

        for (int i = 0; i <= numSlices; i++) {
            Vertex v;
            v.position =
                Vector3::Transform(Vector3(topRadius, 0.5f * height, 0.0f),
                                   Matrix::CreateRotationY(dTheta * float(i)));
            v.normal = v.position - Vector3(0.0f, 0.5f * height, 0.0f);
            v.normal.Normalize();
            v.texcoord = Vector2(float(i) / numSlices, 0.0f);

            vertices.push_back(v);
        }

        vector<uint32_t> &indices = meshData.indices;

        for (int i = 0; i < numSlices; i++) {

            indices.push_back(i);
            indices.push_back(i + numSlices + 1);
            indices.push_back(i + 1 + numSlices + 1);

            indices.push_back(i);
            indices.push_back(i + 1 + numSlices + 1);
            indices.push_back(i + 1);

        }

        return meshData;
    }

    MeshData GeometryGenerator::MakeSphere(const float radius, const int numSlices,
        const int numStacks) {

        const float dTheta = -XM_2PI / float(numSlices);
        const float dPhi = -XM_PI / float(numStacks);

        MeshData meshData;

        vector<Vertex> &vertices = meshData.vertices;

        for (int j = 0; j <= numStacks; j++) {
            
            Vector3 stackStartPoint = Vector3::Transform(
            Vector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * j));

            for (int i = 0; i <= numSlices; i++) {
                Vertex v;

                v.position = Vector3::Transform(
                    stackStartPoint, Matrix::CreateRotationY(dTheta *float(i)));

                v.normal = v.position;
                v.normal.Normalize();
                v.texcoord =
                    Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks);

                vertices.push_back(v);
            }
        }

        vector<uint32_t> &indices = meshData.indices;

        for (int j = 0; j < numStacks; j++) {

            const int offset = (numSlices + 1) * j;

            for (int i = 0; i < numSlices; i++) {

                indices.push_back(offset + i);
                indices.push_back(offset + i + numSlices + 1);
                indices.push_back(offset + i + 1 + numSlices + 1);

                indices.push_back(offset + i);
                indices.push_back(offset + i + 1 + numSlices + 1);
                indices.push_back(offset + i + 1);
            }
        }

        return meshData;
    }

    MeshData GeometryGenerator::MakeIcosahedron() { 
        
        const float X = 0.525731f;
        const float Z = 0.850651f;

        MeshData newMesh;

        vector<Vector3> pos = {
            Vector3(-X, 0.0f, Z), Vector3(X, 0.0f, Z),   Vector3(-X, 0.0f, -Z),
            Vector3(X, 0.0f, -Z), Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
            Vector3(0.0f, -Z, X), Vector3(0.0f, -Z, -X), Vector3(Z, X, 0.0f),
            Vector3(-Z, X, 0.0f), Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f)};

         for (size_t i = 0; i < pos.size(); i++) {
             Vertex v;
             v.position = pos[i];
             v.normal = v.position;
             v.normal.Normalize();

             newMesh.vertices.push_back(v);
         }

         newMesh.indices = {1,4,0,4,9,0,4,5,9,8,5,4,1,8,4,
         1,10,8,10,3,8,8,3,5,3,2,5,3,7,2,
         3,10,7,10,6,7,6,11,7,6,0,11,6,1,0,
         10,1,6,11,0,9,2,11,9,5,2,9,11,2,7};

         return newMesh;
    }

    MeshData GeometryGenerator::MakeTetrahedron() {
        
        const float a = 1.0f;
        const float x = sqrt(3.0f) / 3.0f * a;
        const float d = sqrt(3.0f) / 6.0f * a;
        const float h = sqrt(6.0f) / 3.0f * a;

        vector<Vector3> points = {
            {0.0f,x,0.0f},
            {-0.5f * a, -d, 0.0f},
            {+0.5f * a, -d, 0.0f},
            {0.0f, 0.0f,h}
        };

        Vector3 center = Vector3(0.0f);

        for (int i = 0; i < 4; i++) {
            center += points[i];
        }
        center /= 4.0f;

        for (int i = 0; i < 4; i++) {
           points[i] -= center;
        }

        MeshData meshData;

        for (int i = 0; i < points.size(); i++) {
            
            Vertex v;
            v.position = points[i];
            v.normal = v.position;
            v.normal.Normalize();

            meshData.vertices.push_back(v);
        }

        meshData.indices = {0, 1, 2, 3, 2, 1, 0, 3, 1, 0, 2, 3};

        return meshData;
    }

    MeshData GeometryGenerator::SubdivideToSphere(const float radius,
        MeshData meshData) {

        using namespace DirectX;
        using DirectX::SimpleMath::Matrix;
        using DirectX::SimpleMath::Vector3;

        for (auto &v : meshData.vertices) {
            v.position = v.normal * radius;
        }

        auto ProjectVertex = [&](Vertex &v) {
            v.normal = v.position;
            v.normal.Normalize();
            v.position = v.normal * radius;
        };

        auto UpdateFaceNormal = [](Vertex &v0, Vertex &v1, Vertex &v2) {
            auto faceNormal =
                (v1.position - v0.position).Cross(v2.position - v0.position);
            faceNormal.Normalize();
            v0.normal = faceNormal;
            v1.normal = faceNormal;
            v2.normal = faceNormal;
        };

        MeshData newMesh;
        uint32_t count = 0;
        for (size_t i = 0; i < meshData.indices.size(); i += 3) {
            size_t i0 = meshData.indices[i];
            size_t i1 = meshData.indices[i + 1];
            size_t i2 = meshData.indices[i + 2];

            Vertex v0 = meshData.vertices[i0];
            Vertex v1 = meshData.vertices[i1];
            Vertex v2 = meshData.vertices[i2];

            Vertex v3;
            v3.position = (v0.position + v2.position) * 0.5f;
            v3.texcoord = (v0.texcoord + v2.texcoord) * 0.5f;
            ProjectVertex(v3);

            
            Vertex v4;
            v4.position = (v0.position + v1.position) * 0.5f;
            v4.texcoord = (v0.texcoord + v1.texcoord) * 0.5f;
            ProjectVertex(v4);

            
            Vertex v5;
            v5.position = (v1.position + v2.position) * 0.5f;
            v5.texcoord = (v1.texcoord + v2.texcoord) * 0.5f;
            ProjectVertex(v5);

            newMesh.vertices.push_back(v4);
            newMesh.vertices.push_back(v1);
            newMesh.vertices.push_back(v5);

            newMesh.vertices.push_back(v0);
            newMesh.vertices.push_back(v4);
            newMesh.vertices.push_back(v3);

            newMesh.vertices.push_back(v3);
            newMesh.vertices.push_back(v4);
            newMesh.vertices.push_back(v5);

            newMesh.vertices.push_back(v3);
            newMesh.vertices.push_back(v5);
            newMesh.vertices.push_back(v2);

            for (uint32_t j = 0; j < 12; j++) {
                newMesh.indices.push_back(j + count);
            }
            count += 12;
        }
        return newMesh;
    }

    vector<MeshData> GeometryGenerator::ReadFromFile(std::string basePath,
        std::string filename) {

        using namespace DirectX;

        ModelLoader modelLoader;
        modelLoader.Load(basePath, filename);
        vector<MeshData> &meshes = modelLoader.meshes;

        Vector3 vmin(1000, 1000, 1000);
        Vector3 vmax(-1000, -1000, -1000);
        for (auto &mesh : meshes) {
            for (auto &v : mesh.vertices) {
                vmin.x = XMMin(vmin.x, v.position.x);
                vmin.y = XMMin(vmin.y, v.position.y);
                vmin.z = XMMin(vmin.z, v.position.z);
                vmax.x = XMMax(vmax.x, v.position.x);
                vmax.y = XMMax(vmax.y, v.position.y);
                vmax.z = XMMax(vmax.z, v.position.z);
            }
        }

        float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
        float dl = XMMax(XMMax(dx, dy), dz);
        float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
              cz = (vmax.z + vmin.z) * 0.5f;

        for (auto &mesh : meshes) {
            for (auto &v : mesh.vertices) {
                v.position.x = (v.position.x - cx) / dl;
                v.position.y = (v.position.y - cy) / dl;
                v.position.z = (v.position.z - cz) / dl;
            }
        }

        return meshes;
    }
    }