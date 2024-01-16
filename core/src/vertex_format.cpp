#include <okami/vertex_format.hpp>

using namespace okami;

int okami::GetSize(ValueType v) {
	switch (v) {
		case ValueType::FLOAT32:
			return 4;
		case ValueType::FLOAT16:
			return 2;
		case ValueType::INT8:
			return 1;
		case ValueType::INT16:
			return 2;
		case ValueType::INT32:
			return 4;
		case ValueType::UINT8:
			return 1;
		case ValueType::UINT16:
			return 2;
		case ValueType::UINT32:
			return 4;
		default:
			return -1;
	}
}

VertexFormat VertexFormat::Position() {
	VertexFormat layout;
	layout.position = 0;

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX)
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormat VertexFormat::PositionColor() {
	VertexFormat layout;
	layout.position = 0;
	layout.colors = {1};

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 4, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormat VertexFormat::PositionUVNormalTangent() {
	VertexFormat layout;
	layout.position = 0;
	layout.uvs = {1};
	layout.normal = 2;
	layout.tangent = 3;

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 2, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(2, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(3, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormat VertexFormat::PositionUVNormal() {
	VertexFormat layout;
	layout.position = 0;
	layout.uvs = {1};
	layout.normal = 2;

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 2, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(2, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormat VertexFormat::PositionUVNormalTangentBitangent() {
	VertexFormat layout;
	layout.position = 0;
	layout.uvs = {1};
	layout.normal = 2;
	layout.tangent = 3;
	layout.bitangent = 4;

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 2, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(2, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(3, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(4, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormat VertexFormat::PositionUV() {
	VertexFormat layout;
	layout.position = 0;
	layout.uvs = {1};

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 2, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}


Error VertexFormat::CheckValid() const {
	int numElements = static_cast<int>(elements.size());

	OKAMI_ERR_RETURN_IF(position >= numElements, RuntimeError{"Invalid position index!"});
	OKAMI_ERR_RETURN_IF(normal >= numElements, RuntimeError{"Invalid normal index!"});
	OKAMI_ERR_RETURN_IF(tangent >= numElements, RuntimeError{"Invalid tangent index!"});
	OKAMI_ERR_RETURN_IF(bitangent >= numElements, RuntimeError{"Invalid bitangent index!"});

	for (auto uv : uvs) {
		OKAMI_ERR_RETURN_IF(uv >= numElements, RuntimeError{"Invalid UV index!"});
		OKAMI_ERR_RETURN_IF(elements[uv].numComponents != 2, RuntimeError{"UV does not have 2 components!"});
	}

	for (auto color : colors) {
		OKAMI_ERR_RETURN_IF(color >= numElements, RuntimeError{"Invalid color index!"});
		OKAMI_ERR_RETURN_IF(elements[color].numComponents != 4, RuntimeError{"Color does not have 4 components!"});
	}

	for (auto uvw : uvws) {
		OKAMI_ERR_RETURN_IF(uvw >= numElements, RuntimeError{"Invalid UVW index!"});
		OKAMI_ERR_RETURN_IF(elements[uvw].numComponents != 3, RuntimeError{"Color does not have 4 components!"});
	}

	return {};
}

Error VertexFormat::AutoLayout() {
	bool useAutoStride = false;
    bool useAutoOffset = false;

    for (auto const& layoutElement : elements) {
        if (useAutoOffset) {
            OKAMI_ERR_RETURN_IF(layoutElement.relativeOffset != LAYOUT_ELEMENT_AUTO_OFFSET,
                RuntimeError{"All layout elements must have auto offset enabled if one does!"});
        } else {
            useAutoOffset = (layoutElement.relativeOffset == LAYOUT_ELEMENT_AUTO_OFFSET);
        }

        if (useAutoStride) {
            OKAMI_ERR_RETURN_IF(layoutElement.stride != LAYOUT_ELEMENT_AUTO_STRIDE,
                RuntimeError{"All layout elements must have auto offset enabled if one does!"});
        } else {
            useAutoStride = (layoutElement.stride == LAYOUT_ELEMENT_AUTO_STRIDE);
        }
    }

	OKAMI_ERR_RETURN_IF(useAutoOffset != useAutoStride, 
		RuntimeError{"Auto offset and auto stride must both be enabled if one is!"});
	
	int offset = 0;
	for (auto& layoutElement : elements) {
		if (useAutoOffset) {
			layoutElement.relativeOffset = offset;
		}
		offset += layoutElement.numComponents * GetSize(layoutElement.valueType);
	}
	if (useAutoStride) {
		for (auto& layoutElement : elements) {
			layoutElement.stride = offset;
		}
	}

	return {};
}