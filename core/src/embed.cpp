#include <okami/embed.hpp>

using namespace okami;

Expected<std::string_view> okami::EmbeddedFileLoader::Find(const std::filesystem::path& source) const {
    auto searchSource = std::filesystem::relative(source, "");
    
    // Search internal shaders first
    auto it = _internalShaders.find(searchSource);
    OKAMI_EXP_RETURN_IF(it == _internalShaders.end(), InvalidPathError{source});
    return it->second;
}

void okami::EmbeddedFileLoader::Add(const embedded_file_loader_t& factory) {
    factory(_internalShaders);
}

okami::EmbeddedFileLoader::EmbeddedFileLoader(const embedded_file_loader_t& factory) {
    Add(factory);
}
