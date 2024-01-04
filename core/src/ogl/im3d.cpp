#include <okami/ogl/im3d.hpp>
#include <okami/ogl/utils.hpp>

using namespace okami;

Expected<GLIm3dRenderer> okami::GLIm3dRenderer::Create() {
	GLIm3dRenderer result;

	Error err;
    {	
		ShaderPreprocessorConfig vsConfig{ .defines = {
			{"VERTEX_SHADER", ""},
			{"POINTS", ""}
		}};
		ShaderPreprocessorConfig fsConfig{ .defines = {
			{"FRAGMENT_SHADER", ""},
			{"POINTS", ""}
		}};
        auto vs = OKAMI_EXP_UNWRAP(
			LoadEmbeddedGLShader("im3d.glsl", GL_VERTEX_SHADER, vsConfig), err);
		auto fs = OKAMI_EXP_UNWRAP(
			LoadEmbeddedGLShader("im3d.glsl", GL_FRAGMENT_SHADER, fsConfig), err);

		result.points.shader = OKAMI_EXP_GL(glCreateProgram());
		OKAMI_EXP_GL(glAttachShader(*result.points.shader, *vs));
		OKAMI_EXP_GL(glAttachShader(*result.points.shader, *fs));
		OKAMI_EXP_GL(glLinkProgram(*result.points.shader));		
	}

	{	
		ShaderPreprocessorConfig vsConfig{ .defines = {
			{"VERTEX_SHADER", ""},
			{"LINES", ""}
		}};
		ShaderPreprocessorConfig gsConfig{ .defines = {
			{"GEOMETRY_SHADER", ""},
			{"LINES", ""}
		}};
		ShaderPreprocessorConfig fsConfig{ .defines = {
			{"FRAGMENT_SHADER", ""},
			{"LINES", ""}
		}};
        auto vs = OKAMI_EXP_UNWRAP(
			LoadEmbeddedGLShader("im3d.glsl", GL_VERTEX_SHADER, vsConfig), err);
		auto gs = OKAMI_EXP_UNWRAP(
			LoadEmbeddedGLShader("im3d.glsl", GL_GEOMETRY_SHADER, gsConfig), err);
		auto fs = OKAMI_EXP_UNWRAP(
			LoadEmbeddedGLShader("im3d.glsl", GL_FRAGMENT_SHADER, fsConfig), err);

		result.lines.shader = OKAMI_EXP_GL(glCreateProgram());
		OKAMI_EXP_GL(glAttachShader(*result.lines.shader, *vs));
		OKAMI_EXP_GL(glAttachShader(*result.lines.shader, *gs));
		OKAMI_EXP_GL(glAttachShader(*result.lines.shader, *fs));
		OKAMI_EXP_GL(glLinkProgram(*result.lines.shader));		
	}

	{	
		ShaderPreprocessorConfig vsConfig{ .defines = {
			{"VERTEX_SHADER", ""},
			{"TRIANGLES", ""}
		}};
		ShaderPreprocessorConfig fsConfig{ .defines = {
			{"FRAGMENT_SHADER", ""},
			{"TRIANGLES", ""}
		}};
        auto vs = OKAMI_EXP_UNWRAP(
			LoadEmbeddedGLShader("im3d.glsl", GL_VERTEX_SHADER, vsConfig), err);
		auto fs = OKAMI_EXP_UNWRAP(
			LoadEmbeddedGLShader("im3d.glsl", GL_FRAGMENT_SHADER, fsConfig), err);

		result.triangles.shader = OKAMI_EXP_GL(glCreateProgram());
		OKAMI_EXP_GL(glAttachShader(*result.triangles.shader, *vs));
		OKAMI_EXP_GL(glAttachShader(*result.triangles.shader, *fs));
		OKAMI_EXP_GL(glLinkProgram(*result.triangles.shader));		
	}

	OKAMI_EXP_GL(glGenBuffers(1, &*result.vertexBuffer));
	OKAMI_EXP_GL(glGenVertexArrays(1, &*result.vertexArray));	
	OKAMI_EXP_GL(glBindVertexArray(*result.vertexArray));
	OKAMI_EXP_GL(glBindBuffer(GL_ARRAY_BUFFER, *result.vertexBuffer));
	OKAMI_EXP_GL(glEnableVertexAttribArray(0));
	OKAMI_EXP_GL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Im3d::VertexData), (GLvoid*)offsetof(Im3d::VertexData, m_positionSize)));
	OKAMI_EXP_GL(glEnableVertexAttribArray(1));
	OKAMI_EXP_GL(glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Im3d::VertexData), (GLvoid*)offsetof(Im3d::VertexData, m_color)));
	OKAMI_EXP_GL(glBindVertexArray(0));

	auto getUniformLocations = [](Shader& shader) -> Error {
		shader.uViewport = OKAMI_ERR_GL(
			glGetUniformLocation(*shader.shader, "uViewport"));
		shader.uViewProjMatrix = OKAMI_ERR_GL(
			glGetUniformLocation(*shader.shader, "uViewProjMatrix"));
		return {};
	};

	err |= getUniformLocations(result.points);
	err |= getUniformLocations(result.lines);
	err |= getUniformLocations(result.triangles);
	OKAMI_EXP_RETURN(err);

	return result;
}

Error okami::GLIm3dRenderer::Draw(
	Im3d::Context& context,
	int viewportWidth, int viewportHeight) const {
	
	// Typical pipeline state: enable alpha blending, disable depth test and backface culling.
	OKAMI_ERR_GL(glEnable(GL_BLEND));
	OKAMI_ERR_GL(glBlendEquation(GL_FUNC_ADD));
	OKAMI_ERR_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	OKAMI_ERR_GL(glEnable(GL_PROGRAM_POINT_SIZE));
	OKAMI_ERR_GL(glDisable(GL_DEPTH_TEST));
	OKAMI_ERR_GL(glDisable(GL_CULL_FACE));

	OKAMI_ERR_GL(glViewport(0, 0, (GLsizei)viewportWidth, (GLsizei)viewportHeight));
		
	for (Im3d::U32 i = 0, n = context.getDrawListCount(); i < n; ++i)
	{
		const Im3d::DrawList& drawList = context.getDrawLists()[i];
	
		GLenum prim;
		Shader const* sh;
		switch (drawList.m_primType)
		{
			case Im3d::DrawPrimitive_Points:
				prim = GL_POINTS;
				sh = &points;
				OKAMI_ERR_GL(glDisable(GL_CULL_FACE)); // points are view-aligned
				break;
			case Im3d::DrawPrimitive_Lines:
				prim = GL_LINES;
				sh = &lines;
				OKAMI_ERR_GL(glDisable(GL_CULL_FACE)); // lines are view-aligned
				break;
			case Im3d::DrawPrimitive_Triangles:
				prim = GL_TRIANGLES;
				sh = &triangles;
				//glAssert(glEnable(GL_CULL_FACE)); // culling valid for triangles, but optional
				break;
			default:
				return OKAMI_ERR_MAKE(RuntimeError{"Invalid primitive type!"});
		};

		OKAMI_ERR_GL(glBindVertexArray(*vertexArray));
		OKAMI_ERR_GL(glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer));
		OKAMI_ERR_GL(glBufferData(GL_ARRAY_BUFFER, 
			(GLsizeiptr)drawList.m_vertexCount * sizeof(Im3d::VertexData), 
			(GLvoid*)drawList.m_vertexData, 
			GL_STREAM_DRAW));

		Im3d::AppData& ad = context.getAppData();
		OKAMI_ERR_GL(glUseProgram(*sh->shader));
		OKAMI_ERR_GL(glUniform2f(sh->uViewport, 
			ad.m_viewportSize.x, ad.m_viewportSize.y));

		GLfloat tmp[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		OKAMI_ERR_GL(glUniformMatrix4fv(sh->uViewProjMatrix, 1, false, tmp));
		OKAMI_ERR_GL(glDrawArrays(prim, 0, (GLsizei)drawList.m_vertexCount));
	}

	return {};
}