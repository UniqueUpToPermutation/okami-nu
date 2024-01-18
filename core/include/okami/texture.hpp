#pragma once

#include <okami/vertex_format.hpp>

#include <filesystem>

namespace okami {
    namespace texture {
        enum class Dimension {
            Buffer,
            Texture1D,
            Texture1DArray,
            Texture2D,
            Texture2DArray,
            Texture3D,
            TextureCube,
            TextureCubeArray
        };

        inline bool IsArray(Dimension dim) {
            if (dim == Dimension::Texture1D ||
                dim == Dimension::Texture2D ||
                dim == Dimension::Texture3D) {
                return false;
            } else {
                return true;
            }
        }

        struct Format {
            uint32_t channels;
            ValueType valueType;
            bool isNormalized;
            bool isLinear;

            static Format UNKNOWN();
            static Format RGBA32_FLOAT();
            static Format RGBA32_UINT();
            static Format RGBA32_SINT();
            static Format RGB32_FLOAT();
            static Format RGB32_UINT();
            static Format RGB32_SINT();
            static Format R32_FLOAT();
            static Format R32_SINT();
            static Format R32_UINT();
            static Format RG32_FLOAT();
            static Format RG32_UINT();
            static Format RG32_SINT();
            static Format RGBA8_UINT();
            static Format RGBA8_UNORM();
            static Format RGBA8_SINT();
            static Format RGBA8_SNORM();
            static Format SRGBA8_UNORM();
            static Format R8_SINT();
            static Format R8_UINT();
            static Format R8_SNORM();
            static Format R8_UNORM();
            static Format RG8_SINT();
            static Format RG8_UINT();
            static Format RG8_SNORM();
            static Format RG8_UNORM();
            static Format RGB8_SINT();
            static Format RGB8_UINT();
            static Format RGB8_SNORM();
            static Format RGB8_UNORM();
            static Format R16_SINT();
            static Format R16_UINT();
            static Format R16_SNORM();
            static Format R16_UNORM();
            static Format RG16_SINT();
            static Format RG16_UINT();
            static Format RG16_SNORM();
            static Format RG16_UNORM();
            static Format RGBA16_SINT();
            static Format RGBA16_UINT();
            static Format RGBA16_SNORM();
            static Format RGBA16_UNORM();

            inline uint32_t GetPixelByteSize() const {
                return GetSize(valueType) * channels;
            }
        };

        struct LoadParams {
            bool isSRGB = false;
            bool generateMips = true;
            
            inline LoadParams() {
            }

            inline LoadParams(
                bool isSRGB, 
                bool generateMips) :
                isSRGB(isSRGB),
                generateMips(generateMips) {
            }

            template <class Archive>
            void save(Archive& archive) const {
                archive(generateMips);
                archive(isSRGB);
            }

            template <class Archive>
            void load(Archive& archive) {
                archive(generateMips);
                archive(isSRGB);
            }
        };

        uint32_t MipCount(
            const uint32_t width, 
            const uint32_t height);

        uint32_t MipCount(
            const uint32_t width, 
            const uint32_t height, 
            const uint32_t depth);

        struct SubResDataDesc {
            uint64_t depthStride;
            uint64_t srcOffset;
            uint64_t stride;
            uint64_t length;
            uint32_t mip;
            uint32_t slice;
        };

        struct Desc {
            Dimension type;
            uint32_t width = 1;
            uint32_t height = 1;
            uint32_t arraySizeOrDepth = 1;
            Format format;
            uint32_t mipLevels = 1;
            uint32_t sampleCount = 1;

            inline uint32_t GetPixelByteSize() const {
                return format.GetPixelByteSize();
            }

            uint32_t GetByteSize() const;
            std::vector<SubResDataDesc> GetSubresourceDescs() const;

            uint32_t GetMipCount() const {
                if (mipLevels == 0) {
                    return MipCount(width, height, arraySizeOrDepth);
                } else 
                    return mipLevels;
            }

            uint32_t GetDepth() const {
                if (IsArray(type)) {
                    return 1u;
                } else {
                    return arraySizeOrDepth;
                }
            }

            uint32_t GetArraySize() const {
                if (IsArray(type)) {
                    return arraySizeOrDepth;
                } else {
                    return 1u;
                }
            }
        };

        struct Buffer {
            Desc desc;
            std::vector<uint8_t> data;

            void GenerateMips();
            static Buffer Alloc(const Desc& desc);
            static Buffer Load(
                const std::filesystem::path& path,
                const LoadParams& params);

            inline Desc GetDesc() const {
                return desc;
            }
        };

        inline Buffer Load(
            const std::filesystem::path& path,
            const LoadParams& params) {
            return Buffer::Load(path, params);
        }

        namespace prefabs {
            static Buffer SolidColor(
                uint32_t width,
                uint32_t height,
                std::array<float, 4> color);
        };
    }

    using TextureBuffer = texture::Buffer;
    using TextureDesc = texture::Desc;
    using TextureLoadParams = texture::LoadParams;
    using TextureFormat = texture::Format;
}