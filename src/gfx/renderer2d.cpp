#include <rdge/gfx/renderer2d.hpp>
#include <rdge/gfx/renderable2d.hpp>
#include <rdge/gfx/shaders/shader.hpp>
#include <rdge/gfx/color.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/hints.hpp>

// TODO Testing for dummy texture
#include <rdge/assets/surface.hpp>
#include <SDL.h>
// TODO End testing

#include <GL/glew.h>

#include <algorithm>
#include <sstream>

using namespace rdge;
using namespace gfx;

namespace {

    constexpr rdge::uint32 VERTEX_SIZE = sizeof(vertex_data);
    constexpr rdge::uint32 SPRITE_SIZE = VERTEX_SIZE * 4;

    // Vertex attribute indices (maps to vertex_data struct)
    constexpr rdge::uint32 VATTR_VERTEX_INDEX = 0;
    constexpr rdge::uint32 VATTR_UV_INDEX     = 1;
    constexpr rdge::uint32 VATTR_TID_INDEX    = 2;
    constexpr rdge::uint32 VATTR_COLOR_INDEX  = 3;

} // anonymous namespace

Renderer2D::Renderer2D (rdge::uint16 max_sprite_count)
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
        std::ostringstream ss;
        ss << "Invalid sprite count.  Allowed range is "
           << "[1-" << MAX_SUPPORTED_SPRITE_COUNT << "]"
           << "value=" << max_sprite_count;

        RDGE_THROW(ss.str());
    }

    // define sizes used for buffer allocation
    rdge::uint32 buffer_size  = static_cast<rdge::uint32>(max_sprite_count) * SPRITE_SIZE;
    rdge::uint32 indices_size = static_cast<rdge::uint32>(max_sprite_count) * 6;

    DLOG(
         "Constructing Renderer2D max_sprites=" + std::to_string(max_sprite_count) +
         " buffer_size=" + std::to_string(buffer_size)
        );

    // push an identity matrix on the transformation stack so all submissions
    // wihtout a transformation will essentially be a no-op
    m_transformationStack.push_back(rdge::math::mat4::identity());
    m_currentTransformation = &m_transformationStack.back();

    m_vao = opengl::CreateVertexArray();
    m_vbo = opengl::CreateBuffer();

    opengl::BindVertexArray(m_vao);
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // Allocate the full size (empty) buffer
    opengl::SetBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);

    opengl::EnableVertexAttribute(VATTR_VERTEX_INDEX);
    opengl::EnableVertexAttribute(VATTR_UV_INDEX);
    opengl::EnableVertexAttribute(VATTR_TID_INDEX);
    opengl::EnableVertexAttribute(VATTR_COLOR_INDEX);

    opengl::SetVertexAttributePointer(
                                      VATTR_VERTEX_INDEX,
                                      3,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, vertex))
                                     );

    opengl::SetVertexAttributePointer(
                                      VATTR_UV_INDEX,
                                      2,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, uv))
                                     );

    opengl::SetVertexAttributePointer(
                                      VATTR_TID_INDEX,
                                      1,
                                      GL_INT,
                                      //GL_UNSIGNED_INT,
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

    opengl::SetVertexAttributePointer(
                                      VATTR_COLOR_INDEX,
                                      4,
                                      GL_UNSIGNED_BYTE,
                                      true,
                                      VERTEX_SIZE,
                                      reinterpret_cast<void*>(offsetof(vertex_data, color))
                                     );

    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    // TODO: I like the idea of creating a helper function for this.
    //       Not sure where to put it, but it could be something like
    //       Sprite::GetIBO(sprite_count).  Also, this is a good
    //       candidate for SIMD
    IndexBufferData ibo_data(new rdge::uint32[indices_size]);
    rdge::uint32 offset = 0;
    for (rdge::uint32 i = 0; i < indices_size; i += 6)
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

    opengl::UnbindVertexArrays();

    // work-around for supporting sprites that don't have loaded textures.  Issue was
    // the fragment shader had to have a conditional, which is universally a bad idea.
    // So we create a dummy 1px white texture that when multiplied by the color will
    // simply result in the color being rendered.
    rdge::uint32 dummy_pixel = 0xFFFFFFFF;
    auto dummy_sdl_surface = SDL_CreateRGBSurfaceFrom(static_cast<void*>(&dummy_pixel),
                                                      1, 1, 32, 4, 0, 0, 0, 0);
    auto dummy_surface = std::make_shared<rdge::Surface>(dummy_sdl_surface);
    auto dummy_texture = std::make_shared<rdge::Texture>(*dummy_surface);
    RegisterTexture(dummy_texture);
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
Renderer2D::RegisterTexture (std::shared_ptr<Texture>& texture)
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
    if (!texture || texture->unit_id != Texture::INVALID_UNIT_ID)
    {
        return;
    }

    // TODO I could create a SamplerArray class which holds and manages the textures/unit ids
    auto size = static_cast<uint32>(m_textures.size());
    if (size >= (Shader::MaxFragmentShaderUnits() - 1))
    {
        RDGE_THROW(
                   "Unable to register texture.  Max limit of " +
                   std::to_string(Shader::MaxFragmentShaderUnits()) +
                   " already reached."
                  );
    }

    texture->unit_id = static_cast<uint32>(size);
    m_textures.emplace_back(texture);

    ILOG("Added texture at index " + std::to_string(size) +
         " width=" + std::to_string(texture->width) +
         " height=" + std::to_string(texture->height));
}

void
Renderer2D::PrepSubmit (void)
{
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_buffer = reinterpret_cast<vertex_data*>(opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

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

    using namespace rdge::math;

    auto pos   = renderable->Position();
    auto size  = renderable->Size();
    auto uv    = renderable->UV();
    //auto tid   = static_cast<rdge::int32>(renderable->TextureUnitID());
    auto tid   = static_cast<int32>(renderable->TextureUnitID());
    auto color = static_cast<rdge::uint32>(renderable->Color());

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
    // TODO: I don't see why this is necessary.  'PrepSubmit' could be renamed, and this could
    //       be moved to the flush phase
    opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
    opengl::UnbindBuffers(GL_ARRAY_BUFFER);
}

void
Renderer2D::Flush (void)
{
    for (auto& texture : m_textures)
    {
        texture->Activate();
    }

    opengl::BindVertexArray(m_vao);
    m_ibo.Bind();

    opengl::DrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);

    m_ibo.Unbind();
    opengl::UnbindVertexArrays();

    m_indexCount = 0;
}

void
Renderer2D::PushTransformation (const rdge::math::mat4& matrix, bool override)
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
