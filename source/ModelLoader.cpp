#include "ModelLoader.h"

#include <filesystem>

namespace fs = std::filesystem;

static std::string GetMaterialTexturePath(aiMaterial *mat, aiTextureType type) {
    aiString tex;
    if (mat->GetTextureCount(type) <= 0)
        return "";

    if (mat->GetTexture(type, 0, &tex) != AI_SUCCESS)
        return "";

    std::string s = tex.C_Str();

    if (!s.empty() && s[0] == '*')
        return "";

    return s;
}


static std::string ToLower(std::string s) {
    for (auto &c : s)
        c = (char)tolower((unsigned char)c);
    return s;
}

static std::string Key(std::string s) { 
    s = ToLower(s);
    std::string out;
    out.reserve(s.size());
    for (unsigned char ch : s) {
        if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9'))
            out.push_back((char)ch);
    }
    return out;
}

static std::string ExtractTokenMonster2L(const std::string &s) {
    std::string t = ToLower(s);
    auto pos = t.find("monster2_l_");
    if (pos == std::string::npos)
        return "";

    size_t end = pos;
    while (end < t.size()) {
        char c = t[end];
        if (!(isalnum((unsigned char)c) || c == '_'))
            break;
        end++;
    }
    return t.substr(pos, end - pos);
}

static std::string FindBaseColorForMaterial(const std::string &modelFullPath,
                                            int materialIndex,
                                            const std::string &materialName) {
    namespace fs = std::filesystem;
    fs::path dir = fs::path(modelFullPath).parent_path();

    auto contains = [](const std::string &a, const std::string &b) {
        return a.find(b) != std::string::npos;
    };

    const std::string matIdxKey =
        ToLower("monster2_l_" + std::to_string(materialIndex));

    std::string token = ExtractTokenMonster2L(materialName);

    std::string best;
    std::string defaultPath; 
    std::string firstAny; 

    for (auto &p : fs::directory_iterator(dir)) {
        if (!p.is_regular_file())
            continue;

        std::string ext = ToLower(p.path().extension().string());
        if (ext != ".png" && ext != ".jpg" && ext != ".jpeg")
            continue;

        std::string name = ToLower(p.path().filename().string());
        if (!contains(name, "basecolor"))
            continue;

        if (firstAny.empty())
            firstAny = p.path().string();
        if (contains(name, "default_basecolor"))
            defaultPath = p.path().string();

        if (contains(name, matIdxKey)) {
            best = p.path().string();
            break;
        }
    }

    if (!best.empty())
        return best;
    if (!defaultPath.empty())
        return defaultPath;
    if (!firstAny.empty())
        return firstAny;

    return "";
}

static std::string FindTextureForMaterialByKeyword(
    const std::string &modelFullPath, int materialIndex,
    const std::string &materialName, const std::vector<std::string> &keywords) {

    namespace fs = std::filesystem;
    fs::path dir = fs::path(modelFullPath).parent_path();

    auto contains = [](const std::string &a, const std::string &b) {
        return a.find(b) != std::string::npos;
    };

    std::string matIdxKey = ToLower(materialName);

    std::string best;
    std::string firstAny;

    for (auto &p : fs::directory_iterator(dir)) {
        if (!p.is_regular_file())
            continue;

        std::string ext = ToLower(p.path().extension().string());
        if (ext != ".png" && ext != ".jpg")
            continue;

        std::string name = ToLower(p.path().filename().string());

        bool ok = true;
        for (auto &k : keywords) {
            if (!contains(name, ToLower(k))) {
                ok = false;
                break;
            }
        }
        if (!ok)
            continue;

        if (firstAny.empty())
            firstAny = p.path().string();

        if (contains(name, matIdxKey)) {
            best = p.path().string();
            break;
        }
    }

    return !best.empty() ? best : firstAny;
}

namespace hlab {

using namespace DirectX::SimpleMath;

void ModelLoader::Load(std::string basePath, std::string filename) {

    this->basePath = basePath;

    Assimp::Importer importer;

    std::filesystem::path fullPath =
        std::filesystem::path(this->basePath) / filename;

    this->modelFullPath = fullPath;

    const aiScene *pScene = importer.ReadFile(

        fullPath.string(),
        aiProcess_Triangulate | aiProcess_ConvertToLeftHanded |
            aiProcess_FlipUVs | aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

    if (!pScene || !pScene->mRootNode ||
        (pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) {
        std::cerr << "[Assimp] ReadFile failed: " << importer.GetErrorString()
                  << "\n";
        return;
    }

    DirectX::SimpleMath::Matrix tr = DirectX::SimpleMath::Matrix::Identity;
    ProcessNode(pScene->mRootNode, pScene, tr);

    for (auto &m : this->meshes) {

        vector<Vector3> normalsTemp(m.vertices.size(), Vector3(0.0f));
        vector<float> weightsTemp(m.vertices.size(), 0.0f);

        for (int i = 0; i < m.indices.size(); i += 3) {

            int idx0 = m.indices[i];
            int idx1 = m.indices[i + 1];
            int idx2 = m.indices[i + 2];

            auto v0 = m.vertices[idx0];
            auto v1 = m.vertices[idx1];
            auto v2 = m.vertices[idx2];

            auto faceNormal =
                (v1.position - v0.position).Cross(v2.position - v0.position);

            normalsTemp[idx0] += faceNormal;
            normalsTemp[idx1] += faceNormal;
            normalsTemp[idx2] += faceNormal;
            weightsTemp[idx0] += 1.0f;
            weightsTemp[idx1] += 1.0f;
            weightsTemp[idx2] += 1.0f;
        }

        for (int i = 0; i < m.vertices.size(); i++) {
            if (weightsTemp[i] > 0.0f) {
                m.vertices[i].normal = normalsTemp[i] / weightsTemp[i];
                m.vertices[i].normal.Normalize();
            }
        }
    }
}

void ModelLoader::ProcessNode(aiNode *node, const aiScene *scene, Matrix tr) {

    Matrix m;
    ai_real *temp = &node->mTransformation.a1;
    float *mTemp = &m._11;

    for (int t = 0; t < 16; t++) {
        mTemp[t] = float(temp[t]);
    }
    m = m.Transpose() * tr;

    for (UINT i = 0; i < node->mNumMeshes; i++) {

        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        auto newMesh = this->ProcessMesh(mesh, scene);

        for (auto &v : newMesh.vertices) {
            v.position = DirectX::SimpleMath::Vector3::Transform(v.position, m);
        }

        meshes.push_back(newMesh);
    }

    for (UINT i = 0; i < node->mNumChildren; i++) {
        this->ProcessNode(node->mChildren[i], scene, m);
    }
}

MeshData ModelLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene) {

    MeshData newMesh;

    if (!mesh)
        return newMesh;

    if (!mesh->HasTextureCoords(0)) {
        std::cout << "[Assimp] mesh has NO UV0. materialIndex="
                  << mesh->mMaterialIndex << "\n";
    }

    if (mesh->mNumVertices == 0 || mesh->mNumVertices > 5'000'000) {
        std::cout << "[Assimp] suspicious vertex count: " << mesh->mNumVertices
                  << "\n";
        return newMesh;
    }
    if (mesh->mNumFaces == 0 || mesh->mNumFaces > 10'000'000) {
        std::cout << "[Assimp] suspicious face count: " << mesh->mNumFaces
                  << "\n";
        return newMesh;
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;


    vertices.reserve(mesh->mNumVertices);
    indices.reserve((size_t)mesh->mNumFaces * 3);

    for (UINT i = 0; i < mesh->mNumVertices; i++) {

        Vertex vertex{};

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        /*
        if (mesh->mTextureCoords[0]) {
            vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
            vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
        }*/

        if (mesh->mNormals) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0]) {
            vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
            vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
        } else {
            vertex.texcoord = {0, 0};
        }

        if (mesh->mTangents && mesh->mBitangents) {
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;

            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        } else {

            vertex.tangent = {1, 0, 0};
            vertex.bitangent = {0, 1, 0};
        }

        vertices.push_back(vertex);
    }

    for (UINT i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    newMesh.vertices = std::move(vertices);
    newMesh.indices = std::move(indices);

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::string matName = material->GetName().C_Str();

        newMesh.baseColorFilename = FindTextureForMaterialByKeyword(
            this->modelFullPath.string(), (int)mesh->mMaterialIndex, matName,
            {"basecolor"});

        newMesh.normalFilename = FindTextureForMaterialByKeyword(
            this->modelFullPath.string(), (int)mesh->mMaterialIndex, matName,
            {"normal"});

        newMesh.ormFilename = FindTextureForMaterialByKeyword(
            this->modelFullPath.string(), (int)mesh->mMaterialIndex, matName,
            {"roughness"});
    }

        if (newMesh.ormFilename.empty()) 
        {

        }

    return newMesh;
}
}
 