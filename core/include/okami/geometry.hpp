#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <filesystem>
#include <cstring>
#include <vector>
#include <cstdint>
#include <array>

#include <okami/vertex_format.hpp>
#include <okami/okami.hpp>

namespace okami {
 	struct BoundingBox {
		glm::vec3 mLower;
		glm::vec3 mUpper;
	};

	struct BoundingBox2D {
		glm::vec2 mLower;
		glm::vec2 mUpper;
	};

	namespace geometry {

		struct Attribs {
			uint32_t numVertices = 0;
		};

		struct IndexedAttribs {
			ValueType indexType = ValueType::UNDEFINED;
			uint32_t numIndices = 0;
		};

		struct Desc {
			VertexFormatInfo layout;
			Topology topology = Topology::TRIANGLE_LIST;
			Attribs attribs;
			IndexedAttribs indexedAttribs;
			bool isIndexed;
		};

		template <typename IndexType = uint32_t,
				typename Vec2Type = glm::vec2,
				typename Vec3Type = glm::vec3,
				typename Vec4Type = glm::vec4>
		struct Data {
			std::vector<IndexType> indices;
			std::vector<Vec3Type> positions;
			std::vector<std::vector<Vec2Type>> uvs;
			std::vector<Vec3Type> normals;
			std::vector<Vec3Type> tangents;
			std::vector<Vec3Type> bitangents;
			std::vector<std::vector<Vec4Type>> colors;
			std::vector<std::vector<Vec3Type>> uvws;

			template <typename Archive>
			void serialize(Archive& arr) {
				arr(indices);
				arr(positions);
				arr(uvs);
				arr(normals);
				arr(tangents);
				arr(bitangents);
				arr(colors);
				arr(uvws);
			}

			static Data<
				IndexType, 
				Vec2Type, 
				Vec3Type, 
				Vec4Type> Load(
					const std::filesystem::path& path,
					const VertexFormatInfo& layout);
		};

		template <typename IndexType = uint32_t,
				typename Vec2Type = glm::vec2,
				typename Vec3Type = glm::vec3,
				typename Vec4Type = glm::vec4>
		struct DataView {
			size_t vertexCount;
			size_t indexCount;

			IndexType const* indices;
			Vec3Type const* positions;
			std::vector<Vec2Type const*> uvs;
			Vec3Type const* normals;
			Vec3Type const* tangents;
			Vec3Type const* bitangents;
			std::vector<Vec4Type const*> colors;
			std::vector<Vec3Type const*> uvws;

			inline DataView(
				const Data<IndexType, Vec2Type, Vec3Type, Vec4Type>& data) :
				vertexCount(data.positions.size()),
				indexCount(data.indices.size()),
				indices(data.indices.size() > 0 ? &data.indices[0] : nullptr),
				positions(data.positions.size() > 0 ? &data.positions[0] : nullptr),
				normals(data.normals.size() > 0 ? &data.normals[0] : nullptr),
				tangents(data.tangents.size() > 0 ? &data.tangents[0] : nullptr),
				bitangents(data.bitangents.size() > 0 ? &data.bitangents[0] : nullptr) {

				for (auto& uv : data.uvs) {
					uvs.emplace_back(&uv[0]);
				}

				for (auto& uvw : data.uvws) {
					uvws.emplace_back(&uvw[0]);
				}
				
				for (auto& color : data.colors)  {
					colors.emplace_back(&color[0]);
				}
			}

			DataView(
				size_t vertexCount,
				size_t indexCount,
				const IndexType indices[],
				const Vec3Type positions[],
				const std::vector<const Vec2Type*>& uvs = {},
				const Vec3Type normals[] = nullptr,
				const Vec3Type tangents[] = nullptr,
				const Vec3Type bitangents[] = nullptr,
				const std::vector<const Vec4Type*>& colors = {},
				const std::vector<const Vec3Type*>& uvws = {}) : 
				vertexCount(vertexCount),
				indexCount(indexCount),
				indices(indices),
				positions(positions),
				uvs(uvs),
				normals(normals),
				tangents(tangents),
				bitangents(bitangents),
				colors(colors),
				uvws(uvws) {
			}

			DataView(
				size_t vertexCount,
				size_t indexCount,
				const IndexType indices[],
				const Vec3Type positions[],
				const Vec2Type uvs[] = nullptr,
				const Vec3Type normals[] = nullptr,
				const Vec3Type tangents[] = nullptr,
				const Vec3Type bitangents[] = nullptr,
				const Vec4Type colors[] = nullptr,
				const Vec3Type uvws[] = nullptr) :
				DataView(vertexCount,
					indexCount,
					indices,
					positions,
					std::vector<const Vec2Type*>{uvs},
					normals,
					tangents,
					bitangents,
					std::vector<const Vec4Type*>{colors},
					std::vector<const Vec2Type*>{uvws}) {
			}
			
			DataView(size_t vertexCount,
				const Vec3Type positions[],
				const std::vector<const Vec2Type*>& uvs = {},
				const Vec3Type normals[] = nullptr,
				const Vec3Type tangents[] = nullptr,
				const Vec3Type bitangents[] = nullptr,
				const std::vector<const Vec4Type*>& colors = {},
				const std::vector<const Vec3Type*>& uvws = {}) :
				DataView(vertexCount,
					0,
					nullptr,
					positions,
					uvs,
					normals,
					tangents,
					bitangents,
					colors,
					uvws) {
			}

			DataView(size_t vertexCount,
				const Vec3Type positions[],
				const Vec2Type uvs[] = nullptr,
				const Vec3Type normals[] = nullptr,
				const Vec3Type tangents[] = nullptr,
				const Vec3Type bitangents[] = nullptr,
				const Vec4Type colors[] = nullptr,
				const Vec3Type uvws[] = nullptr) : 
				DataView(vertexCount,
					0,
					nullptr,
					positions,
					{uvs},
					normals,
					tangents,
					bitangents,
					{colors},
					{uvws}) {
			}

			bool HasIndices() const {
				return indices != nullptr;
			}

			bool HasPositions() const {
				return positions != nullptr;
			}

			bool HasNormals() const {
				return normals != nullptr;
			}

			bool HasTangents() const {
				return tangents != nullptr;
			}

			bool HasBitangents() const {
				return bitangents != nullptr;
			}
		};

		class Geometry {
		private:
			Desc desc;
			std::vector<BufferData> vertexBuffers;
			BufferData indexBuffer;
			BoundingBox boundingBox;

		public:
			const std::vector<BufferData>& GetVertexBuffers() const {
				return vertexBuffers;
			}

			const BufferData& GetIndexBuffer() const {
				return indexBuffer;
			}

			template <typename I3T, typename V2T, typename V3T, typename V4T>
			static Geometry Pack(const VertexFormatInfo& layout,
				const DataView<I3T, V2T, V3T, V4T>& data);
			template <typename I3T, typename V2T, typename V3T, typename V4T>
			static Geometry Pack(const VertexFormatInfo& layout,
				const Data<I3T, V2T, V3T, V4T>& data);

			template <typename I3T = uint32_t, 
				typename V2T = glm::vec2, 
				typename V3T = glm::vec3, 
				typename V4T = glm::vec4>
			static Data<I3T, V2T, V3T, V4T> Unpack(const Geometry& buffer);

			static Geometry Load(
				const std::filesystem::path& path,
				const VertexFormatInfo& layout);

			inline Geometry(
				const std::filesystem::path& path,
				const VertexFormatInfo& layout) {
				*this = Load(path, layout);
			}

			template <typename I3T, typename V2T, typename V3T, typename V4T>
			inline Geometry(const VertexFormatInfo& layout,
				const DataView<I3T, V2T, V3T, V4T>& data) {
				*this = Pack<I3T, V2T, V3T, V4T>(layout, data);
			}

			template <typename I3T, typename V2T, typename V3T, typename V4T>
			inline Geometry(const VertexFormatInfo& layout,
				const Data<I3T, V2T, V3T, V4T>& data) :
				Geometry(layout, DataView<I3T, V2T, V3T, V4T>(data)) {
			}

			Geometry(const Geometry&) = delete;
			Geometry& operator=(const Geometry&) = delete;

			Geometry(Geometry&&) = default;
			Geometry& operator=(Geometry&&) = default;

			Geometry() = default;

			Geometry Duplicate();
			Geometry ToLayout(const VertexFormatInfo& format);

			inline void Dealloc() {
				vertexBuffers.clear();
				indexBuffer.bytes.clear();
			}

			inline const BoundingBox& GetBounds() const {
				return boundingBox;
			}

			inline const Desc& GetDesc() const {
				return desc;
			}

			inline VertexFormatInfo const& GetLayout() const {
				return desc.layout;
			}
		};

		namespace prefabs {
			Geometry MaterialBall(const VertexFormatInfo& layout);
			Geometry Box(const VertexFormatInfo& layout);
			Geometry Sphere(const VertexFormatInfo& layout);
			Geometry BlenderMonkey(const VertexFormatInfo& layout);
			Geometry Torus(const VertexFormatInfo& layout);
			Geometry Plane(const VertexFormatInfo& layout);
			Geometry StanfordBunny(const VertexFormatInfo& layout);
			Geometry UtahTeapot(const VertexFormatInfo& layout);
		};

		Geometry Load(
			const std::filesystem::path& path, 
			const VertexFormatInfo& layout);

		template <typename T>
		struct V4Packer;

		template <typename T>
		struct V3Packer;

		template <typename T>
		struct V2Packer;

		template <typename T>
		struct I3Packer;

		template <>
		struct V4Packer<glm::vec4> {
			static constexpr size_t kStride = 1;

			inline static void Pack(float* dest, const glm::vec4* src) {
				dest[0] = src->x;
				dest[1] = src->y;
				dest[2] = src->z;
				dest[3] = src->w;
			}

			inline static void Unpack(glm::vec4* dest, const float* src) {
				dest->x = src[0];
				dest->y = src[1];
				dest->z = src[2];
				dest->w = src[3];
			}
		};

		template <>
		struct V4Packer<float> {
			static constexpr size_t kStride = 4;

			inline static void Pack(float* dest, const float* src) {
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
				dest[3] = src[3];
			}

			inline static void Unpack(float* dest, const float* src) {
				Pack(dest, src);
			}
		};

		template <>
		struct V3Packer<glm::vec3> {
			static constexpr size_t kStride = 1;

			inline static void Pack(float* dest, const glm::vec3* src) {
				dest[0] = src->x;
				dest[1] = src->y;
				dest[2] = src->z;
			}

			inline static void Unpack(glm::vec3* dest, const float* src) {
				dest->x = src[0];
				dest->y = src[1];
				dest->z = src[2];
			}
		};

		template <>
		struct V3Packer<float> {
			static constexpr size_t kStride = 3;

			inline static void Pack(float* dest, const float* src) {
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
			}

			inline static void Unpack(float* dest, const float* src) {
				Pack(dest, src);
			}
		};

		template <>
		struct V2Packer<glm::vec2> {
			static constexpr size_t kStride = 1;

			inline static void Pack(float* dest, const glm::vec2* src) {
				dest[0] = src->x;
				dest[1] = src->y;
			}

			inline static void Unpack(glm::vec2* dest, const float* src) {
				dest->x = src[0];
				dest->y = src[1];
			}
		};

		template <>
		struct V2Packer<float> {
			static constexpr size_t kStride = 2;

			inline static void Pack(float* dest, const float* src) {
				dest[0] = src[0];
				dest[1] = src[1];
			}

			inline static void Unpack(float* dest, const float* src) {
				Pack(dest, src);
			}
		};

		template <>
		struct I3Packer<uint32_t> {
			static constexpr size_t kStride = 3;

			inline static void Pack(uint32_t* dest, const uint32_t* src) {
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
			}

			inline static void Unpack(uint32_t* dest, const uint32_t* src) {
				Pack(dest, src);
			}
		};

		template <
			typename destT, 
			typename srcT, 
			void(*MoveFunc)(destT*, const srcT*)>
		void ArraySliceCopy(destT* dest, const srcT* source, 
			size_t destStride, size_t srcStride, size_t count) {
			size_t destIndex = 0;
			size_t srcIndex = 0;
			
			for (size_t i = 0; i < count; ++i, destIndex += destStride, srcIndex += srcStride) {
				(*MoveFunc)(&dest[destIndex], &source[srcIndex]);
			}
		}

		template <
			typename destT, 
			typename srcT, 
			void(*MoveFunc)(destT*, const srcT*)>
		void ArraySliceCopyToBytes(uint8_t* dest, const srcT* source, 
			size_t destStrideBytes, size_t srcStride, size_t count) {
			size_t destIndex = 0;
			size_t srcIndex = 0;
			
			for (size_t i = 0; i < count; ++i, destIndex += destStrideBytes, srcIndex += srcStride) {
				(*MoveFunc)(reinterpret_cast<destT*>(&dest[destIndex]), &source[srcIndex]);
			}
		}

		template <
			typename destT, 
			typename srcT, 
			void(*MoveFunc)(destT*, const srcT*)>
		void ArraySliceCopyFromBytes(destT* dest, const uint8_t* source, 
			size_t destStride, size_t srcStrideBytes, size_t count) {
			size_t destIndex = 0;
			size_t srcIndex = 0;
			
			for (size_t i = 0; i < count; ++i, destIndex += destStride, srcIndex += srcStrideBytes) {
				(*MoveFunc)(&dest[destIndex], reinterpret_cast<const srcT*>(&source[srcIndex]));
			}
		}

		template <typename T, size_t dim>
		void ArraySliceCopy(T* dest, const T* source,
			size_t destStride, size_t srcStride, size_t count) {
			size_t destIndex = 0;
			size_t srcIndex = 0;
			for (size_t i = 0; i < count; ++i, destIndex += destStride, srcIndex += srcStride) {
				for (size_t component = 0; component < dim; ++component) {
					dest[destIndex + component] = source[srcIndex + component];
				}
			}
		}

		template <typename T, size_t dim>
		void ArraySliceBoundingBox(T* arr, 
			size_t stride, size_t count, 
			std::array<T, dim>& lower, std::array<T, dim>& upper) {
			std::fill(upper.begin(), upper.end(), -std::numeric_limits<T>::infinity());
			std::fill(lower.begin(), lower.end(), std::numeric_limits<T>::infinity());

			size_t srcIndex = 0;
			for (size_t i = 0; i < count; ++i, srcIndex += stride) {
				for (size_t component = 0; component < dim; ++component) {
					upper[component] = std::max<T>(upper[component], arr[srcIndex]);
					lower[component] = std::min<T>(lower[component], arr[srcIndex]);
				}
			}
		}

		template <typename T, size_t dim>
		void ArraySliceBoundingBoxBytes(uint8_t* arr, 
			size_t stride, size_t count, 
			std::array<T, dim>& lower, 
			std::array<T, dim>& upper) {
			std::fill(upper.begin(), upper.end(), -std::numeric_limits<T>::infinity());
			std::fill(lower.begin(), lower.end(), std::numeric_limits<T>::infinity());

			size_t srcIndex = 0;
			for (size_t i = 0; i < count; srcIndex += stride, ++i) {
				auto arr_cast = reinterpret_cast<T*>(&arr[srcIndex]);
				for (size_t component = 0; component < dim; ++component) {
					upper[component] = std::max<T>(upper[component], arr_cast[component]);
					lower[component] = std::min<T>(lower[component], arr_cast[component]);
				}
			}
		}

		inline BoundingBox ArraySliceBoundingBox(float* arr,
			size_t stride, size_t count) {
			std::array<float, 3> upper;
			std::array<float, 3> lower;

			ArraySliceBoundingBox<float, 3>(arr, stride, count, lower, upper);

			BoundingBox result;
			result.mLower = glm::vec3(lower[0], lower[1], lower[2]);
			result.mUpper = glm::vec3(upper[0], upper[1], upper[2]);
			return result;
		}

		inline BoundingBox ArraySliceBoundingBoxBytes(uint8_t* arr,
			size_t stride, size_t count) {
			std::array<float, 3> upper;
			std::array<float, 3> lower;

			ArraySliceBoundingBoxBytes<float, 3>(arr, stride, count, lower, upper);

			BoundingBox result;
			result.mLower = glm::vec3(lower[0], lower[1], lower[2]);
			result.mUpper = glm::vec3(upper[0], upper[1], upper[2]);
			return result;
		}

		template <typename destT, size_t componentCount>
		void ArraySliceFill(destT* dest,
			const destT& value,
			size_t stride,
			size_t vertex_count) {

			for (size_t i = 0; i < vertex_count; i += stride) {
				for (size_t component = 0; component < componentCount; ++component) {
					dest[i + component] = value;
				}
			}
		}

		template <typename destT, size_t componentCount>
		void ArraySliceFillBytes(uint8_t* dest,
			const destT& value,
			size_t stride,
			size_t vertex_count) {

			for (size_t i = 0; i < vertex_count; i += stride) {
				auto dest_cast = reinterpret_cast<destT*>(&dest[i]);
				for (size_t component = 0; component < componentCount; ++component) {
					dest_cast[component] = value;
				}
			}
		}

		// Given a vertex layout, compute the offsets, strides, and channel sizes
		// of each of the geometry elements in the layout
		void ComputeLayoutProperties(
			size_t vertex_count,
			const VertexFormatInfo& layout,
			std::vector<size_t>& offsets,
			std::vector<size_t>& strides,
			std::vector<size_t>& channel_sizes);

		// A structure that holds all indexing strides and offsets for
		// different geometry elements (i.e., position, normal, etc.).
		struct PackIndexing {
			int positionOffset = -1;
			int positionChannel = -1;
			int positionStride = -1;

			std::vector<int> uvOffsets = {};
			std::vector<int> uvChannels = {};
			std::vector<int> uvStrides = {};

			int normalOffset = -1;
			int normalChannel = -1;
			int normalStride = -1;

			int tangentOffset = -1;
			int tangentChannel = -1;
			int tangentStride = -1;

			int bitangentOffset = -1;
			int bitangentChannel = -1;
			int bitangentStride = -1;

			std::vector<int> colorOffsets = {};
			std::vector<int> colorChannels = {};
			std::vector<int> colorStrides = {};

			std::vector<int> uvwOffsets = {};
			std::vector<int> uvwChannels = {};
			std::vector<int> uvwStrides = {};

			std::vector<size_t> channelSizes;

			static PackIndexing From(const VertexFormatInfo& layout,
				size_t vertex_count);
		};

		template <typename I3T, typename V2T, typename V3T, typename V4T>
		Geometry Geometry::Pack(const VertexFormatInfo& layout,
			const DataView<I3T, V2T, V3T, V4T>& data) {

			Geometry result;

			if (layout.CheckValid().IsError()) {
				throw std::runtime_error("Invalid vertex format!");
			}

			size_t vertex_count = data.vertexCount;
			size_t index_count = data.indexCount;

			auto indexing = PackIndexing::From(layout, vertex_count);

			auto& channel_sizes = indexing.channelSizes;
			uint32_t channelCount = (uint32_t)channel_sizes.size();
		
			std::vector<std::vector<uint8_t>> vert_buffers(channelCount);
			for (uint32_t i = 0; i < channelCount; ++i)
				vert_buffers[i] = std::vector<uint8_t>(channel_sizes[i]);

			std::vector<uint8_t> indx_buffer_raw(index_count * sizeof(uint32_t));
			uint32_t* indx_buffer = (uint32_t*)(&indx_buffer_raw[0]);

			BoundingBox aabb;

			bool hasNormals = data.normals != nullptr;
			bool hasPositions = data.positions != nullptr;
			bool hasBitangents = data.bitangents != nullptr;
			bool hasTangents = data.tangents != nullptr;

			if (indexing.positionOffset >= 0) {
				auto& channel = vert_buffers[indexing.positionChannel];
				auto arr = &channel[indexing.positionOffset];
				if (hasPositions) {
					ArraySliceCopyToBytes<float, V3T, &V3Packer<V3T>::Pack>(
						arr, &data.positions[0], indexing.positionStride, V3Packer<V3T>::kStride, vertex_count);
					aabb = ArraySliceBoundingBoxBytes(arr, (size_t)indexing.positionStride, vertex_count);
				} else {
					ArraySliceFillBytes<float, 3>(arr, 0.0f, indexing.positionStride, vertex_count);
					aabb.mLower = glm::vec3(0.0f, 0.0f, 0.0f);
					aabb.mUpper = glm::vec3(0.0f, 0.0f, 0.0f);
				}
			}

			for (uint32_t iuv = 0; iuv < indexing.uvChannels.size(); ++iuv) {
				auto& vertexChannel = vert_buffers[indexing.uvChannels[iuv]];
				auto arr = &vertexChannel[indexing.uvOffsets[iuv]];
				if (iuv < data.uvs.size()) {
					ArraySliceCopyToBytes<float, V2T, &V2Packer<V2T>::Pack>(
						arr, &data.uvs[iuv][0], indexing.uvStrides[iuv], V2Packer<V2T>::kStride, vertex_count);
				} else {
					ArraySliceFillBytes<float, 2>(arr, 0.0f, indexing.uvStrides[iuv], vertex_count);
				}
			}

			if (indexing.normalOffset >= 0) {
				auto& channel = vert_buffers[indexing.normalChannel];
				auto arr = &channel[indexing.normalOffset];
				if (hasNormals) {
					ArraySliceCopyToBytes<float, V3T, &V3Packer<V3T>::Pack>(
						arr, &data.normals[0], indexing.normalStride, V3Packer<V3T>::kStride, vertex_count);
				} else {
					ArraySliceFillBytes<float, 3>(arr, 0.0f, indexing.normalStride, vertex_count);
				}
			}

			if (indexing.tangentOffset >= 0) {
				auto& channel = vert_buffers[indexing.tangentChannel];
				auto arr = &channel[indexing.tangentOffset];
				if (hasTangents) {
					ArraySliceCopyToBytes<float, V3T, &V3Packer<V3T>::Pack>(
						arr, &data.tangents[0], indexing.tangentStride, 
							V3Packer<V3T>::kStride, vertex_count);
				} else {
					ArraySliceFillBytes<float, 3>(arr, 0.0f, indexing.tangentStride, vertex_count);
				}
			}

			if (indexing.bitangentOffset >= 0) {
				auto& channel = vert_buffers[indexing.bitangentChannel];
				auto arr = &channel[indexing.bitangentOffset];
				if (hasBitangents) {
					ArraySliceCopyToBytes<float, V3T, &V3Packer<V3T>::Pack>(
						arr, &data.bitangents[0], indexing.bitangentStride, 
							V3Packer<V3T>::kStride, vertex_count);
				} else {
					ArraySliceFillBytes<float, 3>(arr, 0.0f, indexing.bitangentStride, vertex_count);
				}
			}

			for (uint32_t icolor = 0; icolor < indexing.colorChannels.size(); ++icolor) {
				auto& vertexChannel = vert_buffers[indexing.colorChannels[icolor]];
				auto arr = &vertexChannel[indexing.colorOffsets[icolor]];
				if (icolor < data.colors.size()) {
					ArraySliceCopyToBytes<float, V4T, &V4Packer<V4T>::Pack>(
						arr, &data.colors[icolor][0], indexing.colorStrides[icolor], 
							V4Packer<V4T>::kStride, vertex_count);
				} else {
					ArraySliceFillBytes<float, 4>(arr, 1.0f, indexing.colorStrides[icolor], vertex_count);
				}
			}

			for (uint32_t iuvw = 0; iuvw < indexing.uvwChannels.size(); ++iuvw) {
				auto& vertexChannel = vert_buffers[indexing.uvwChannels[iuvw]];
				auto arr = &vertexChannel[indexing.uvwOffsets[iuvw]];
				if (iuvw < data.uvws.size()) {
					ArraySliceCopyToBytes<float, V3T, &V3Packer<V3T>::Pack>(
						arr, &data.uvws[iuvw][0], indexing.uvwStrides[iuvw],
							V3Packer<V3T>::kStride, vertex_count);
				} else {
					ArraySliceFillBytes<float, 3>(arr, 0.0f, indexing.uvwStrides[iuvw], vertex_count);
				}
			}

			if (data.indices != nullptr && data.indexCount > 0) {
				ArraySliceCopy<uint32_t, I3T, &I3Packer<I3T>::Pack>(
					&indx_buffer[0], &data.indices[0], 3, 
					I3Packer<I3T>::kStride, index_count / 3);
			}

			IndexedAttribs indexedAttribs;
			indexedAttribs.indexType = ValueType::UINT32;
			indexedAttribs.numIndices = (uint32_t)(indx_buffer_raw.size() / sizeof(uint32_t));
			
			Attribs attribs;
			attribs.numVertices = vertex_count;

			// Write to output geometry buffer
			result.desc.isIndexed = (data.indices != nullptr && data.indexCount > 0);
			result.desc.attribs = attribs;
			result.desc.indexedAttribs = indexedAttribs;
			result.desc.layout = layout;

			result.vertexBuffers.clear();

			for (uint32_t i = 0; i < channelCount; ++i) {
				BufferData bufferData;
				bufferData.bytes = std::move(vert_buffers[i]);
				result.vertexBuffers.emplace_back(std::move(bufferData));
			}

			BufferData indxBufferData;
			indxBufferData.bytes = std::move(indx_buffer_raw);
			result.indexBuffer = std::move(indxBufferData);

			result.boundingBox = aabb;

			return result;
		}

		template <typename I3T, typename V2T, typename V3T, typename V4T>
		Geometry Geometry::Pack(const VertexFormatInfo& layout,
			const Data<I3T, V2T, V3T, V4T>& data) {
			return Pack(layout, DataView(data));
		}

		template <typename I3T, typename V2T, typename V3T, typename V4T>
		Data<I3T, V2T, V3T, V4T> Geometry::Unpack(const Geometry& buffer) {
			Data<I3T, V2T, V3T, V4T> result;

			size_t vertex_count = 0;
			if (buffer.desc.isIndexed) {
				if (buffer.desc.indexedAttribs.indexType != ValueType::UINT32) {
					throw std::runtime_error("Index type must be VT_UINT32!");
				}

				result.indices.resize(buffer.desc.indexedAttribs.numIndices);
				std::memcpy(&result.indices[0], &buffer.indexBuffer.bytes[0],
					sizeof(uint32_t) * buffer.desc.indexedAttribs.numIndices);
			}
			
			vertex_count = buffer.desc.attribs.numVertices;
			auto& layout = buffer.desc.layout;
			auto indexing = PackIndexing::From(layout, vertex_count);

			if (layout.position >= 0) {
				result.positions.resize(V3Packer<V3T>::kStride * vertex_count);
				auto arr = 
					&buffer.vertexBuffers[indexing.positionChannel].bytes[indexing.positionOffset];
				ArraySliceCopyFromBytes<V3T, float, &V3Packer<V3T>::Unpack>(
					&result.positions[0], arr, V3Packer<V3T>::kStride, 
					indexing.positionStride, vertex_count);
			}

			result.uvs.resize(indexing.uvChannels.size());
			for (uint32_t iuv = 0; iuv < indexing.uvChannels.size(); ++iuv) {
				result.uvs[iuv].resize(V2Packer<V2T>::kStride * vertex_count);
				auto arr = 
					&buffer.vertexBuffers[indexing.uvChannels[iuv]].bytes[indexing.uvOffsets[iuv]];
				ArraySliceCopyFromBytes<V2T, float, &V2Packer<V2T>::Unpack>(
					&result.uvs[iuv][0], arr, V2Packer<V2T>::kStride,
					indexing.uvStrides[iuv], vertex_count);
			}

			if (layout.normal >= 0) {
				result.normals.resize(V3Packer<V3T>::kStride * vertex_count);
				auto arr = 
					&buffer.vertexBuffers[indexing.normalChannel].bytes[indexing.normalOffset];
				ArraySliceCopyFromBytes<V3T, float, &V3Packer<V3T>::Unpack>(
					&result.normals[0], arr, V3Packer<V3T>::kStride,
					indexing.normalStride, vertex_count);
			}

			if (layout.tangent >= 0) {
				result.tangents.resize(V3Packer<V3T>::kStride * vertex_count);
				auto arr = 
					&buffer.vertexBuffers[indexing.tangentChannel].bytes[indexing.tangentOffset];
				ArraySliceCopyFromBytes<V3T, float, &V3Packer<V3T>::Unpack>(
					&result.tangents[0], arr, V3Packer<V3T>::kStride, 
					indexing.tangentStride, vertex_count);
			}

			if (layout.bitangent >= 0) {
				result.bitangents.resize(V3Packer<V3T>::kStride * vertex_count);
				auto arr = 
					&buffer.vertexBuffers[indexing.bitangentChannel].bytes[indexing.bitangentOffset];
				ArraySliceCopyFromBytes<V3T, float, &V3Packer<V3T>::Unpack>(
					&result.bitangents[0], arr, V3Packer<V3T>::kStride, 
					indexing.bitangentStride, vertex_count);
			}

			result.colors.resize(indexing.colorChannels.size());
			for (uint32_t icolor = 0; icolor < indexing.colorChannels.size(); ++icolor) {
				result.colors[icolor].resize(V4Packer<V4T>::kStride * vertex_count);
				auto arr = 
					&buffer.vertexBuffers[indexing.colorChannels[icolor]].bytes[indexing.colorOffsets[icolor]];
				ArraySliceCopyFromBytes<V4T, float, &V4Packer<V4T>::Unpack>(
					&result.colors[icolor][0], arr, V4Packer<V4T>::kStride,
					indexing.colorStrides[icolor], vertex_count);
			}

			return result;
		}

		template <typename IndexType,
				typename Vec2Type,
				typename Vec3Type,
				typename Vec4Type>
		Data<IndexType, 
					Vec2Type, 
					Vec3Type, 
					Vec4Type> 
			Data<IndexType, 
			Vec2Type, 
			Vec3Type, 
			Vec4Type>::Load(
				const std::filesystem::path& path,
				const VertexFormatInfo& layout) {
			auto buffer = Geometry::Load(path, layout);
			return Geometry::Unpack<IndexType, Vec2Type, Vec3Type, Vec4Type>(buffer);
		}
	}

	using Geometry = geometry::Geometry;
	using GeometryDesc = geometry::Desc;
}