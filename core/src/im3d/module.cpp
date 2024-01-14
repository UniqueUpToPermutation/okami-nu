#include <okami/im3d/module.hpp>

#include <im3d.h>

using namespace okami;

Error okami::Im3dModule::Initialize(Registry& registry) const {
    registry.ctx().emplace<Im3d::Context>();
    return {};
}
Error okami::Im3dModule::PreExecute(Registry& registry) const {
    return {};
}
Error okami::Im3dModule::PostExecute(Registry& registry) const {
    return {};
}
Error okami::Im3dModule::Destroy(Registry& registry) const {
    return {};
}
