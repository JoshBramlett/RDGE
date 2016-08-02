#include <rdge/graphics/renderer2d.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/shaders/shader.hpp>
#include <rdge/color.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <GL/glew.h>

#include <algorithm>
#include <sstream>

namespace RDGE {
namespace Graphics {

namespace {

    constexpr RDGE::UInt32 VERTEX_SIZE = sizeof(vertex_data);
    constexpr RDGE::UInt32 SPRITE_SIZE = VERTEX_SIZE * 4;

    // Vertex attribute indices (maps to vertex_data struct)
    constexpr RDGE::UInt32 VATTR_VERTEX_INDEX = 0;
    constexpr RDGE::UInt32 VATTR_UV_INDEX     = 1;
    constexpr RDGE::UInt32 VATTR_TID_INDEX    = 2;
    constexpr RDGE::UInt32 VATTR_COLOR_INDEX  = 3;

} // anonymous namespace

Renderer2D::Renderer2D (RDGE::UInt16 max_sprite_count)
    : m_vao(0)
    , m_vbo(0)
    , m_indexCount(0)
    , m_buffer(nullptr)
    , m_currentTransformation(nullptr)
    , m_submissionCount(0)
    , m_maxSubmissions(max_sprite_count)
{
#ifndef RDGE_DEBUG
    // Avoids release build compiler warnings
    RDGE::Unused(m_maxSubmissions);
#endif

    if (UNLIKELY(max_sprite_count == 0 || max_sprite_count > MAX_SUPPORTED_SPRITE_COUNT))
    {
        std::stringstream ss;
        ss << "Invalid sprite count.  Allowed range is "
           << "[1-" << MAX_SUPPORTED_SPRITE_COUNT << "]"
           << "value=" << max_sprite_count;

        RDGE_THROW(ss.str());
    }

    // define sizes used for buffer allocation
    RDGE::UInt32 buffer_size  = static_cast<RDGE::UInt32>(max_sprite_count) * SPRITE_SIZE;
    RDGE::UInt32 indices_size = static_cast<RDGE::UInt32>(max_sprite_count) * 6;

    DLOG(
         "Constructing Renderer2D max_sprites=" + std::to_string(max_sprite_count) +
         " buffer_size=" + std::to_string(buffer_size)
        );

    // push an identity matrix on the transformation stack so all submissions
    // wihtout a transformation will essentially be a no-op
    m_transformationStack.push_back(RDGE::Math::mat4::identity());
    m_currentTransformation = &m_transformationStack.back();

    m_vao = OpenGL::CreateVertexArray();
    m_vbo = OpenGL::CreateBuffer();

    OpenGL::BindVertexArray(m_vao);
    OpenGL::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // Allocate the full size (empty) buffer
    OpenGL::SetBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);

    OpenGL::EnableVertexAttribute(VATTR_VERTEX_INDEX);
    OpenGL::EnableVertexAttribute(VATTR_UV_INDEX);
    OpenGL::EnableVertexAttribute(VATTR_TID_INDEX);
    OpenGL::EnableVertexAttribute(VATTR_COLOR_INDEX);

    OpenGL::SetVertexAttributePointer(
                                      VATTR_VERTEX_INDEX,
                                      3,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, vertex))
                                     );

    OpenGL::SetVertexAttributePointer(
                                      VATTR_UV_INDEX,
                                      2,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, uv))
                                     );

    OpenGL::SetVertexAttributePointer(
                                      VATTR_TID_INDEX,
                                      1,
                                      GL_INT,
                                      false,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, tid))
                                     );

    // TODO:  Need to figure out what's going on with color.  Supposedly when using
    //        GL_UNSIGNED_BYTE it'll keep the byte order regardless of endian-ness.
    //        The byte order should be GL_RGBA but LUL is creating it as ABGR.  Why
    //        does this work, and test to see what happens when using uint
    //
    //  From StackOverflow:
    //        "GL_RGBA with GL_UNSIGNED_BYTE will store the bytes in RGBA order
    //        regardless whether the computer is little-endian or big-endian."
    //  http://stackoverflow.com/questions/7786187/opengl-texture-upload-unsigned-byte-vs-unsigned-int-8-8-8-8

    OpenGL::SetVertexAttributePointer(
                                      VATTR_COLOR_INDEX,
                                      4,
                                      GL_UNSIGNED_BYTE,
                                      true,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, color))
                                     );

    OpenGL::UnbindBuffers(GL_ARRAY_BUFFER);

    IndexBufferData ibo_data(new RDGE::UInt32[indices_size]);
    RDGE::UInt32 offset = 0;
    for (RDGE::UInt32 i = 0; i < indices_size; i += 6)
    {
        ibo_data[i] = offset;
        ibo_data[i + 1] = offset + 1;
        ibo_data[i + 2] = offset + 2;

        ibo_data[i + 3] = offset + 2;
        ibo_data[i + 4] = offset + 3;
        ibo_data[i + 5] = offset;

        offset += 4;
    }

    m_ibo = IndexBuffer(std::move(ibo_data), indices_size);

    OpenGL::UnbindVertexArrays();
}

Renderer2D::~Renderer2D (void)
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

Renderer2D::Renderer2D (Renderer2D&& rhs) noexcept
    : m_ibo(std::move(rhs.m_ibo))
    , m_indexCount(rhs.m_indexCount)
    , m_buffer(rhs.m_buffer)
    , m_textures(std::move(rhs.m_textures))
    , m_transformationStack(std::move(rhs.m_transformationStack))
    , m_currentTransformation(rhs.m_currentTransformation)
    , m_submissionCount(rhs.m_submissionCount)
    , m_maxSubmissions(rhs.m_maxSubmissions)
{
    // The destructor deletes the OpenGL buffers, therefore we swap the buffer
    // ids so the moved-from object will destroy the buffer it's replacing
    std::swap(m_vao, rhs.m_vao);
    std::swap(m_vbo, rhs.m_vbo);

    rhs.m_buffer = nullptr;
    rhs.m_currentTransformation = nullptr;
}

Renderer2D&
Renderer2D::operator= (Renderer2D&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_ibo = std::move(rhs.m_ibo);
        m_indexCount = rhs.m_indexCount;
        m_buffer = rhs.m_buffer;
        m_textures = std::move(rhs.m_textures);
        m_transformationStack = std::move(rhs.m_transformationStack);
        m_currentTransformation = rhs.m_currentTransformation;
        m_submissionCount = rhs.m_submissionCount;
        m_maxSubmissions = rhs.m_maxSubmissions;

        std::swap(m_vao, rhs.m_vao);
        std::swap(m_vbo, rhs.m_vbo);

        rhs.m_buffer = nullptr;
        rhs.m_currentTransformation = nullptr;
    }

    return *this;
}

void
Renderer2D::RegisterTexture (std::shared_ptr<GLTexture>& texture)
{
    /*
     * There's two ways to implement activating a texture within OpenGL
     *     1) Activate when the texture is registered with the renderer.
     *        The major con is that all textures to be registered must
     *        be generated (assigned a texture id) prior to activating them
     *        (assigning a shader unit id).
     *     2) Activate during render (flush) phase.
     *        This is safer and allows textures to be created at any time,
     *        however, this takes up cycles during the most performance
     *        critical time.  Also, activation is performed every frame,
     *        whereas in option 1 it's only done once.
     *
     * Currently, option 2 is implemented.
     *
     * TODO: Perform a perf test to determine the actual cost of activating
     *       textures within the loop.
     */

    // no texture or already added
    if (!texture || texture->UnitID() >= 0)
    {
        return;
    }

    auto size = static_cast<RDGE::Int32>(m_textures.size());
    if (size >= (Shader::MaxFragmentShaderUnits() - 1))
    {
        RDGE_THROW(
                   "Unable to register texture.  Max limit of " +
                   std::to_string(Shader::MaxFragmentShaderUnits()) +
                   " already reached."
                  );
    }

    texture->SetUnitID(size);
    m_textures.emplace_back(texture->GetSharedPtr());
}

void
Renderer2D::PrepSubmit (void)
{
    OpenGL::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_buffer = reinterpret_cast<vertex_data*>(OpenGL::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    m_submissionCount = 0;
}

void
Renderer2D::Submit (const Renderable2D* renderable)
{
#ifdef RDGE_DEBUG
    if (m_submissionCount++ > m_maxSubmissions)
    {
        RDGE_THROW(
                   "Renderable2D submissions exceeded maximum of " +
                   std::to_string(m_maxSubmissions)
                  );
    }
#endif

    using namespace RDGE::Math;

    auto pos   = renderable->Position();
    auto size  = renderable->Size();
    auto uv    = renderable->UV();
    auto tid   = static_cast<RDGE::Int32>(renderable->TextureUnitID());
    auto color = static_cast<RDGE::UInt32>(renderable->Color());

    m_buffer->vertex = *m_currentTransformation * pos;
    m_buffer->uv = uv[0];
    m_buffer->tid = tid;
    m_buffer->color = color;
    m_buffer++;

    m_buffer->vertex = *m_currentTransformation * vec3(pos.x, pos.y + size.y, pos.z);
    m_buffer->uv = uv[1];
    m_buffer->tid = tid;
    m_buffer->color = color;
    m_buffer++;

    m_buffer->vertex = *m_currentTransformation * vec3(pos.x + size.x, pos.y + size.y, pos.z);
    m_buffer->uv = uv[2];
    m_buffer->tid = tid;
    m_buffer->color = color;
    m_buffer++;

    m_buffer->vertex = *m_currentTransformation * vec3(pos.x + size.x, pos.y, pos.z);
    m_buffer->uv = uv[3];
    m_buffer->tid = tid;
    m_buffer->color = color;
    m_buffer++;

    m_indexCount += 6;
}

void
Renderer2D::EndSubmit (void)
{
    OpenGL::ReleaseBufferPointer(GL_ARRAY_BUFFER);
    OpenGL::UnbindBuffers(GL_ARRAY_BUFFER);
}

void
Renderer2D::Flush (void)
{
    for (auto texture : m_textures)
    {
        texture->Activate();
    }

    OpenGL::BindVertexArray(m_vao);
    m_ibo.Bind();

    OpenGL::DrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);

    m_ibo.Unbind();
    OpenGL::UnbindVertexArrays();

    m_indexCount = 0;
}

void
Renderer2D::PushTransformation (RDGE::Math::mat4 matrix, bool override)
{
    if (override)
    {
        m_transformationStack.push_back(matrix);
    }
    else
    {
        m_transformationStack.push_back(m_transformationStack.back() * matrix);
    }

    m_currentTransformation = &m_transformationStack.back();
}

void
Renderer2D::PopTransformation (void)
{
    if (m_transformationStack.size() > 1)
    {
        m_transformationStack.pop_back();
    }

    m_currentTransformation = &m_transformationStack.back();
}

} // namespace Graphics
} // namespace RDGE
