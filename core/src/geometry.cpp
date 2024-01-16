#include <okami/geometry.hpp>

using namespace okami;
using namespace okami::geometry;

namespace matball {
	#include <matballmesh.hpp>
}

namespace box {
	#include <boxmesh.hpp>
}

namespace bunny {
	#include <bunnymesh.hpp>
}

namespace monkey {
	#include <monkeymesh.hpp>
}

namespace plane {
	#include <planemesh.hpp>
}

namespace sphere {
	#include <spheremesh.hpp>
}

namespace torus {
	#include <torusmesh.hpp>
}

namespace teapot {
	#include <teapotmesh.hpp>
}

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <iostream>

Geometry okami::geometry::prefabs::MaterialBall(const VertexFormat& layout) {
    return Geometry(layout, 
        DataView
            <uint32_t, float, float, float>(
            matball::vertexCount,
            matball::indexCount,
            matball::indices,
            matball::positions,
            matball::uvs,
            matball::normals,
            matball::tangents,
            matball::bitangents));
}

Geometry okami::geometry::prefabs::Box(const VertexFormat& layout) {
    return Geometry(layout, 
        DataView
            <uint32_t, float, float, float>(
            box::vertexCount,
            box::indexCount,
            box::indices,
            box::positions,
            box::uvs,
            box::normals,
            box::tangents,
            box::bitangents));
}

Geometry okami::geometry::prefabs::Sphere(const VertexFormat& layout) {
    return Geometry(layout, 
        DataView
            <uint32_t, float, float, float>(
            sphere::vertexCount,
            sphere::indexCount,
            sphere::indices,
            sphere::positions,
            sphere::uvs,
            sphere::normals,
            sphere::tangents,
            sphere::bitangents));
}

Geometry okami::geometry::prefabs::BlenderMonkey(const VertexFormat& layout) {
    return Geometry(layout, 
        DataView
            <uint32_t, float, float, float>(
            monkey::vertexCount,
            monkey::indexCount,
            monkey::indices,
            monkey::positions,
            monkey::uvs,
            monkey::normals,
            monkey::tangents,
            monkey::bitangents));
}

Geometry okami::geometry::prefabs::Torus(const VertexFormat& layout) {
    return Geometry(layout, 
        DataView
            <uint32_t, float, float, float>(
            torus::vertexCount,
            torus::indexCount,
            torus::indices,
            torus::positions,
            torus::uvs,
            torus::normals,
            torus::tangents,
            torus::bitangents));
}

Geometry okami::geometry::prefabs::Plane(const VertexFormat& layout) {
    return Geometry(layout, 
        DataView
            <uint32_t, float, float, float>(
            plane::vertexCount,
            plane::indexCount,
            plane::indices,
            plane::positions,
            plane::uvs,
            plane::normals,
            plane::tangents,
            plane::bitangents));
}

Geometry okami::geometry::prefabs::StanfordBunny(const VertexFormat& layout) {
    return Geometry(layout, 
        DataView
            <uint32_t, float, float, float>(
            bunny::vertexCount,
            bunny::indexCount,
            bunny::indices,
            bunny::positions,
            bunny::uvs,
            bunny::normals,
            bunny::tangents,
            bunny::bitangents));
}

Geometry okami::geometry::prefabs::UtahTeapot(const VertexFormat& layout) {
    return Geometry(layout, 
        DataView
            <uint32_t, float, float, float>(
            teapot::vertexCount,
            teapot::indexCount,
            teapot::indices,
            teapot::positions,
            teapot::uvs,
            teapot::normals,
            teapot::tangents,
            teapot::bitangents));
}

template <>
struct V3Packer<aiVector3D> {
    static constexpr size_t kStride = 1;

    inline static void Pack(float* dest, const aiVector3D* src) {
        dest[0] = src->x;
        dest[1] = src->y;
        dest[2] = src->z;
    }
};

template <>
struct V2Packer<aiVector3D> {
    static constexpr size_t kStride = 1;

    inline static void Pack(float* dest, const aiVector3D* src) {
        dest[0] = src->x;
        dest[1] = src->y;
    }
};

template <>
struct I3Packer<aiFace> {
    static constexpr size_t kStride = 1;

    inline static void Pack(uint32_t* dest, const aiFace* src) {
        dest[0] = src->mIndices[0];
        dest[1] = src->mIndices[1];
        dest[2] = src->mIndices[2];
    }
};

void okami::geometry::ComputeLayoutProperties(
    size_t vertex_count,
    const VertexFormat& layout,
    std::vector<size_t>& offsets,
    std::vector<size_t>& strides,
    std::vector<size_t>& channel_sizes) {
    offsets.clear();
    strides.clear();

    size_t nVerts = vertex_count;

    auto& layoutElements = layout.elements;

    int channelCount = 0;
    for (auto& layoutItem : layoutElements) {
        channelCount = std::max<int>(channelCount, layoutItem.bufferSlot + 1);
    }

    channel_sizes.resize(channelCount);
    std::vector<size_t> channel_auto_strides(channelCount);

    std::fill(channel_sizes.begin(), channel_sizes.end(), 0u);
    std::fill(channel_auto_strides.begin(), channel_auto_strides.end(), 0u);

    // Compute offsets
    for (auto& layoutItem : layoutElements) {
        uint32_t size = GetSize(layoutItem.valueType) * layoutItem.numComponents;

        if (layoutItem.frequency == InputElementFrequency::PER_VERTEX) {
            uint32_t channel = layoutItem.bufferSlot;

            size_t offset = 0;

            if (layoutItem.relativeOffset == LAYOUT_ELEMENT_AUTO_OFFSET) {
                offset = channel_auto_strides[channel];
            } else {
                offset = layoutItem.relativeOffset;
            }

            offsets.emplace_back(offset);
            channel_auto_strides[channel] += size;
        } else {
            offsets.emplace_back(0);
        }
    }

    // Compute strides
    for (int i = 0; i < offsets.size(); ++i) {
        auto& layoutItem = layoutElements[i];
        size_t offset = offsets[i];
        uint32_t size = GetSize(layoutItem.valueType) * layoutItem.numComponents;

        if (layoutItem.frequency == InputElementFrequency::PER_VERTEX) {
            uint32_t channel = layoutItem.bufferSlot;

            size_t stride = 0;

            if (layoutItem.stride == LAYOUT_ELEMENT_AUTO_STRIDE) {
                stride = channel_auto_strides[channel];
            } else {
                stride = layoutItem.stride;
            }

            strides.emplace_back(stride);

            size_t lastIndex = offset + size + (nVerts - 1) * stride;

            channel_sizes[channel] = std::max<size_t>(channel_sizes[channel], lastIndex);

        } else {
            strides.emplace_back(0);
        }
    }
}

PackIndexing okami::geometry::PackIndexing::From(
    const VertexFormat& layout,
    size_t vertex_count) {

    std::vector<size_t> offsets;
    std::vector<size_t> strides;

    PackIndexing indexing;

    auto& layoutElements = layout.elements;	
    ComputeLayoutProperties(vertex_count, layout, offsets, strides, indexing.channelSizes);

    auto verifyAttrib = [](const LayoutElement& element) {
        if (element.valueType != ValueType::FLOAT32) {
            throw std::runtime_error("Attribute type must be VT_FLOAT32!");
        }
    };

    if (layout.position >= 0) {
        auto& posAttrib = layoutElements[layout.position];
        verifyAttrib(posAttrib);
        indexing.positionOffset = offsets[layout.position];
        indexing.positionChannel = posAttrib.bufferSlot;
        indexing.positionStride = strides[layout.position];
    }

    for (auto& uv : layout.uvs) {
        auto& uvAttrib = layoutElements[uv];
        verifyAttrib(uvAttrib);
        indexing.uvOffsets.emplace_back((int)offsets[uv]);
        indexing.uvChannels.emplace_back(uvAttrib.bufferSlot);
        indexing.uvStrides.emplace_back(strides[uv]);
    }

    if (layout.normal >= 0) {
        auto& normalAttrib = layoutElements[layout.normal];
        verifyAttrib(normalAttrib);
        indexing.normalOffset = offsets[layout.normal];
        indexing.normalChannel = normalAttrib.bufferSlot;
        indexing.normalStride = strides[layout.normal];
    }

    if (layout.tangent >= 0) {
        auto& tangentAttrib = layoutElements[layout.tangent];
        verifyAttrib(tangentAttrib);
        indexing.tangentOffset = offsets[layout.tangent];
        indexing.tangentChannel = tangentAttrib.bufferSlot;
        indexing.tangentStride = strides[layout.tangent];
    }

    if (layout.bitangent >= 0) {
        auto& bitangentAttrib = layoutElements[layout.bitangent];
        verifyAttrib(bitangentAttrib);
        indexing.bitangentOffset = offsets[layout.bitangent];
        indexing.bitangentChannel = bitangentAttrib.bufferSlot;
        indexing.bitangentStride = strides[layout.bitangent];
    }

    for (auto& color : layout.colors) {
        auto& colorAttrib = layoutElements[color];
        verifyAttrib(colorAttrib);
        indexing.colorOffsets.emplace_back(offsets[color]);
        indexing.colorChannels.emplace_back(colorAttrib.bufferSlot);
        indexing.colorStrides.emplace_back(strides[color]);
    }

    for (auto& uvw : layout.uvws) {
        auto& uvwAttrib = layoutElements[uvw];
        verifyAttrib(uvwAttrib);
        indexing.uvwOffsets.emplace_back(offsets[uvw]);
        indexing.uvwChannels.emplace_back(uvwAttrib.bufferSlot);
        indexing.uvwStrides.emplace_back(strides[uvw]);
    }

    return indexing;
}

Geometry okami::geometry::Geometry::ToLayout(const VertexFormat& format) {
    return Geometry::Pack<
        uint32_t,
        glm::vec2,
        glm::vec3,
        glm::vec4>(format, 
            Geometry::Unpack<
                uint32_t,
                glm::vec2,
                glm::vec3,
                glm::vec4>(*this));
}

Geometry okami::geometry::Geometry::Duplicate() {
    Geometry buf;
    buf.boundingBox = boundingBox;
    buf.desc = desc;
    buf.indexBuffer = indexBuffer;
    buf.vertexBuffers = vertexBuffers;
    return buf;
}

Geometry okami::geometry::Geometry::Load(
    const std::filesystem::path& path,
    const VertexFormat& layout) {

    Assimp::Importer importer;

    unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | 
        aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
        aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | 
        aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Quality;

    auto pathStr = path.string();
    const aiScene* scene = importer.ReadFile(pathStr.c_str(), flags);
    
    if (!scene) {
        std::cout << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Failed to load geometry!");
    }

    if (!scene->HasMeshes()) {
        throw std::runtime_error("Geometry has no meshes!");
    }

    size_t nVerts;
    size_t nIndices;

    if (!scene->HasMeshes()) {
        throw std::runtime_error("Assimp scene has no meshes!");
    }

    aiMesh* mesh = scene->mMeshes[0];

    nVerts = mesh->mNumVertices;
    nIndices = mesh->mNumFaces * 3;

    DataView<aiFace, aiVector3D, aiVector3D> data(
        nVerts, nIndices,
        mesh->mFaces,
        mesh->mVertices,
        mesh->mTextureCoords[0],
        mesh->mNormals,
        mesh->mTangents,
        mesh->mBitangents);

    return Geometry::Pack<aiFace, aiVector3D, aiVector3D>(layout, data);
}

Geometry okami::geometry::Load(
    const std::filesystem::path& path, 
    const VertexFormat& layout) {
    return Geometry::Load(path, layout);
}