#include <okami/ogl/renderer.hpp>

using namespace okami;

Error okami::GLRenderer::Initialize() {
    OKAMI_ERR_UNWRAP_INTO(im3d, GLIm3dRenderer::Create());
    return {};
}
Error okami::GLRenderer::Draw(CameraRenderData const& camera, Im3d::Context& context) {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    im3d.Draw(context, camera.viewport.x, camera.viewport.y);
    return {};
}
Error okami::GLRenderer::Destroy() {
    im3d = {};
    return {};
}