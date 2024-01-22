#include <okami/ogl/renderer.hpp>

using namespace okami;

Error okami::GLRenderer::Initialize() {
    OKAMI_ERR_UNWRAP_INTO(im3d, GLIm3dRenderer::Create());
    return {};
}

Error okami::GLRenderer::BeginColorPass() {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return {};
}

Error okami::GLRenderer::DrawIm3d(RenderView const& camera, Im3d::Context& context) {
    im3d.Draw(camera, context);
    return {};
}

Error okami::GLRenderer::Destroy() {
    im3d = {};
    return {};
}