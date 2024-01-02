#include <okami/ogl/embed.hpp>

using namespace okami;

void MakeGLShaderMap(file_map_t&);

std::optional<EmbeddedFileLoader> gFileLoader;

EmbeddedFileLoader const& okami::GetEmbeddedGLShaderSources() {
    if (!gFileLoader) {
        gFileLoader.emplace(EmbeddedFileLoader{&MakeGLShaderMap});
    }
    return *gFileLoader;
}