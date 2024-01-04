constexpr size_t vertexCount = 4;
constexpr size_t indexCount = 6;

constexpr float positions[] = {
-1, 0, -1, 1, 0, -1, 1, 0, 1, -1, 0, 1
};

constexpr float uvs[] = {
0, 1, 1, 1, 1, 0, 0, 0
};

constexpr float normals[] = {
0, 1, -0, 0, 1, -0, 0, 1, -0, 0, 1, -0
};

constexpr float tangents[] = {
1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0
};

constexpr float bitangents[] = {
0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1
};

constexpr uint32_t indices[] = {
3, 2, 1, 3, 1, 0
};
