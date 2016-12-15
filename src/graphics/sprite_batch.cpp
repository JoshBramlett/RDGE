#include <rdge/graphics/sprite_batch.hpp>

#include <sstream>

namespace {

constexpr rdge::uint32 VERTEX_SIZE = sizeof(rdge::vertex_attributes);
constexpr rdge::uint32 SPRITE_SIZE = VERTEX_SIZE * 4;

std::unique_ptr<rdge::Shader> DefaultShader (void)
{
    std::ostringstream vert;
    vert << "#version 330 core\n"
         //
         << "layout (location = " << LAYOUT_LOC_POS   << ") in vec4 position;\n"
         << "layout (location = " << LAYOUT_LOC_UV    << ") in vec2 uv;\n"
         << "layout (location = " << LAYOUT_LOC_TID   << ") in uint tid;\n"
         << "layout (location = " << LAYOUT_LOC_COLOR << ") in vec4 color;\n"
         //
         << "uniform mat4 " << UNI_PROJ_MATRIX << ";\n"
         //
         << "out vertex_attributes"
         << "{\n"
         << "  vec4 pos;\n"
         << "  vec2 uv;\n"
         << "  flat uint tid;\n"
         << "  vec4 color;\n"
         << "} va_data;\n"
         //
         << "void main()\n"
         << "{\n"
         << "  va_data.pos = position;\n"
         << "  va_data.uv = uv;\n"
         << "  va_data.tid = tid;\n"
         << "  va_data.color = color;\n"
         << "  gl_Position = " << UNI_PROJ_MATRIX << " * position;\n"
         << "}\n";

    std::ostringstream frag;
    frag << "#version 330 core\n"
         //
         << "layout (location = 0) out vec4 color;\n"
         //
         << "uniform sampler2D " << UNI_SAMPLER_ARR << "[" << Shader::MaxFragmentShaderUnits() << "];\n"
         //
         << "in vertex_attributes"
         << "{\n"
         << "  vec4 pos;\n"
         << "  vec2 uv;\n"
         << "  flat uint tid;\n"
         << "  vec4 color;\n"
         << "} va_data;\n"
         //
         << "void main()\n"
         << "{\n"
         << "  color = va_data.color * texture(" << UNI_SAMPLER_ARR << "[va_data.tid], va_data.uv);\n"
         << "}\n";

    return std::make_unique<rdge::Shader>(vert.str(), frag.str());
}

} // anonymous namespace

namespace rdge {

SpriteBatch::SpriteBatch (uint32 num_sprites, std::shared_ptr<Shader> shader)
    : m_maxSubmissions(num_sprites)
{
    m_shader = (shader) ? shader : DefaultShader();

    uint32 vbo_size  = num_sprites * SPRITE_SIZE;
    uint32 ibo_count = num_sprites * 6;
    uint32 ibo_size  = ibo_count * sizeof(uint32);

    m_vao = opengl::CreateVertexArray();
    m_vbo = opengl::CreateBuffer();
    m_ibo = opengl::CreateBuffer();

    opengl::BindVertexArray(m_vao);

    // SpriteBatch implements it's buffer object streaming using the orphaning
    // technique, which means for every frame we ask OpenGL to give us a new buffer.
    // Since we request the same size, it's likely (though not guaranteed) that
    // no allocation will take place.  The major drawback of this approach is
    // is it's dependent on the implementation, so performance may differ.
    // https://www.opengl.org/wiki/Buffer_Object_Streaming#Buffer_re-specification
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    opengl::SetBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);

    opengl::EnableVertexAttribute(LAYOUT_LOCATION_POS);
    opengl::SetVertexAttributePointer(LAYOUT_LOCATION_POS,
                                      3,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      static_cast<void*>(offsetof(vertex_attributes, pos)));

    opengl::EnableVertexAttribute(LAYOUT_LOCATION_UV);
    opengl::SetVertexAttributePointer(LAYOUT_LOCATION_UV,
                                      2,
                                      GL_FLOAT,
                                      false,
                                      VERTEX_SIZE,
                                      static_cast<void*>(offsetof(vertex_attributes, uv)));

    opengl::EnableVertexAttribute(LAYOUT_LOCATION_TID);
    opengl::SetVertexAttributePointer(LAYOUT_LOCATION_TID,
                                      1,
                                      GL_UNSIGNED_INT,
                                      false,
                                      VERTEX_SIZE,
                                      static_cast<void*>(offsetof(vertex_attributes, tid)));

    opengl::EnableVertexAttribute(LAYOUT_LOCATION_COLOR);
    opengl::SetVertexAttributePointer(LAYOUT_LOCATION_COLOR,
                                      4,
                                      GL_UNSIGNED_BYTE,
                                      true,
                                      VERTEX_SIZE,
                                      static_cast<void*>(offsetof(vertex_attributes, color)));

    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    // Index buffer - build vertex index table
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    m_indexTable = std::make_unique<uint32[]>(uint32[ibo_count]);
    for (uint32 i = 0, uint32 offset = 0; i < ibo_count; i += 6, offset += 4)
    {
        m_indexTable[i] = offset;
        m_indexTable[i + 1] = offset + 1;
        m_indexTable[i + 2] = offset + 2;

        m_indexTable[i + 3] = offset + 2;
        m_indexTable[i + 4] = offset + 3;
        m_indexTable[i + 5] = offset;
    }

    opengl::SetBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo_size, m_indexTable.get(), GL_STATIC_DRAW);
    opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);

    opengl::UnbindVertexArrays();

    // Fragment shader is required to define a sampler2D array with the element count
    // equal to the maximum texture units available.  A vector is filled with values
    // { 0 ... MAX-1 } and set to the uniform.
    std::vector<int32> texture_units(Shader::MaxFragmentShaderUnits());
    int32 n = 0;
    std::generate(texture_units.begin(), texture_units.end(), [&n]{ return n++; });

    // TODO Hard coding the projection for now till I figure out how I want to deal with
    //      the viewport, aspect ratio, target width/height, etc.
    //      When I start with creating samples/games it may be more clear.  One thing I
    //      need to keep in mind is how to set the width/height of textures.  For a
    //      square sprite (32x32) the target width and height will be modified by the
    //      aspect ratio to keep it square
    //auto vp = opengl::GetViewport();
    m_projection = mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f);

    m_shader->Enable();
    m_shader->SetUniformValue(UNI_PROJ_MATRIX, m_projection);
    m_shader->SetUniformValue(UNI_SAMPLER_ARR, texture_units.size(), texture_units.data());
    m_shader->Disable();

    m_transformStack.push_back(rdge::math::mat4::identity());
    m_transform = &m_transformStack.back();

}

SpriteBatch::~SpriteBatch (void) noexcept
{
    glDeleteBuffers(1, &m_ibo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

SpriteBatch&
SpriteBatch::SpriteBatch (SpriteBatch&& rhs) noexcept
{
    // TODO
}

SpriteBatch&
SpriteBatch::operator= (SpriteBatch&& rhs) noexcept
{
    // TODO
}

void
SpriteBatch::RegisterTexture (Texture& texture)
{

}

void
SpriteBatch::PrepSubmit (void)
{
    m_shader->Enable();
    opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_cursor = static_cast<vertex_attributes*>(opengl::GetBufferPointer(GL_ARRAY_BUFFER,
                                                                        GL_WRITE_ONLY));

    m_submissions = 0;
}

void
//SpriteBatch::Submit (const IRenderable2D& renderable)
SpriteBatch::Submit (const Sprite& renderable)
{
    // IRenderable interface should contain:
    //  1)  std::array<math::vec3>& Verticies
    //  2)  std::array<math::vec2>& UV
    //  3)  uint32 TextureUnitId
    //  4)  rdge::color& Color
    //  * note the reference types to avoid copies
    //
    //  TODO: Either we do the above, which would mean we could keep sprites and
    //        their render target decoupled (which is nice for ease of use).  Or
    //        alternatively the SpriteBatch could create a buffer with the below
    //        data and every sprite would have to "register" with the renderer to
    //        get it's pointer to the vertex data.  The benefit is cache locality,
    //        and I think I could set the type different (GL_STATIC_DRAW).  I'll
    //        have to measure to see how much of an impact this makes.
    //auto vertices        = renderable.Vertices();

    auto& vertices = renderable.Vertices();
    //auto& pos = renderable.position;
    //auto& size = renderable.size;
    //auto& uv = renderable.uv;
    //auto tid = (renderable.texture) ? static_cast<int32>(renderable.texture->UnitID()) : -1;
    //auto color = static_cast<uint32>(renderable.Color());
// TODO
    m_cursor->pos   = *m_transform * pos;
    m_cursor->uv    = uv[0];
    m_cursor->tid   = tid;
    m_cursor->color = color;
    m_cursor++;

    m_cursor->pos   = *m_transform * math::vec3(pos.x, pos.y + size.y, pos.z);
    m_cursor->uv    = uv[1];
    m_cursor->tid   = tid;
    m_cursor->color = color;
    m_cursor++;

    m_cursor->pos   = *m_transform * math::vec3(pos.x + size.x, pos.y + size.y, pos.z);
    m_cursor->uv    = uv[2];
    m_cursor->tid   = tid;
    m_cursor->color = color;
    m_cursor++;

    m_cursor->pos   = *m_transform * math::vec3(pos.x + size.x, pos.y, pos.z);
    m_cursor->uv    = uv[3];
    m_cursor->tid   = tid;
    m_cursor->color = color;
    m_cursor++;

    m_submissions++;
}

void
SpriteBatch::Flush (void)
{
    opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
    opengl::UnbindBuffers(GL_ARRAY_BUFFER);

    for (auto& texture : m_textures)
    {
        texture.Activate();
    }

    opengl::BindVertexArray(m_vao);
    opengl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    //m_ibo.Bind();

    // TODO The last param is a pointer to the index table.  Since I'm not using
    //      an ibo object perhaps I can pass that cursor (and not bind ibo?)
    opengl::DrawElements(GL_TRIANGLES, m_submissions * 6, GL_UNSIGNED_INT, nullptr);

    //m_ibo.Unbind();
    opengl::UnbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
    opengl::UnbindVertexArrays();
}

void
SpriteBatch::PushTransformation (const rdge::math::mat4& matrix) //, bool override)
{
    //if (override)
    //{
        //m_transformationStack.push_back(matrix);
    //}
    //else
    {
        m_transformStack.push_back(m_transformStack.back() * matrix);
    }

    m_transform = &m_transformStack.back();
}

void
SpriteBatch::PopTransformation (void)
{
    if (m_transformStack.size() > 1)
    {
        m_transformStack.pop_back();
    }

    m_transform = &m_transformStack.back();
}

} // namespace rdge
