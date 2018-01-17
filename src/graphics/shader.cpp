#include <rdge/graphics/shader.hpp>
#include <rdge/util/io.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/util/compiler.hpp>

#include <GL/glew.h>
#include <SDL_assert.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>

namespace {

using namespace rdge;

uint32
Compile (ShaderType shader_type, const std::string& source)
{
    uint32 shader = opengl::CreateShader(static_cast<uint32>(shader_type));

    const char* src = source.c_str();
    opengl::SetShaderSource(shader, &src);
    opengl::CompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (RDGE_UNLIKELY(status == GL_FALSE))
    {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> error(length);
        glGetShaderInfoLog(shader, length, &length, &error[0]);
        glDeleteShader(shader);

        std::ostringstream ss;
        ss << "Shader compilation failed."
           << " type=" << shader_type
           << " info=" << error.data();

        GL_THROW(ss.str(), "", 0);
    }

    return shader;
}

uint32
Link (const std::vector<uint32>& shaders)
{
    uint32 program = opengl::CreateProgram();

    for (const auto shader : shaders)
    {
        opengl::AttachShader(program, shader);
    }

    opengl::LinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (RDGE_UNLIKELY(status == GL_FALSE))
    {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> error(length);
        glGetProgramInfoLog(program, length, &length, &error[0]);

        std::ostringstream ss;
        ss << "Program linking failed."
           << " info=" << error.data();

        GL_THROW(ss.str(), "", 0);
    }

    for (const auto shader : shaders)
    {
        opengl::DetachShader(program, shader);
        opengl::DeleteShader(shader);
    }

    return program;
}

} // anonymous namespace

namespace rdge {

Shader::Shader (const std::string& vert_source, const std::string& frag_source)
{
    std::vector<uint32> shaders;

    shaders.emplace_back(Compile(ShaderType::Vertex, vert_source));
    shaders.emplace_back(Compile(ShaderType::Fragment, frag_source));

    m_programId = Link(shaders);
}

Shader::~Shader (void) noexcept
{
    glDeleteProgram(m_programId);
}

Shader::Shader (Shader&& other) noexcept
    : m_uniforms(std::move(other.m_uniforms))
{
    std::swap(m_programId, other.m_programId);
}

Shader&
Shader::operator= (Shader&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_uniforms = std::move(rhs.m_uniforms);
        std::swap(m_programId, rhs.m_programId);
    }

    return *this;
}

void
Shader::Enable (void) const
{
    SDL_assert(m_programId != 0);

    opengl::UseProgram(m_programId);
}

void
Shader::Disable (void) const
{
    opengl::UseProgram(0);
}

void
Shader::SetUniformValue (const std::string& name, int32 value)
{
    opengl::SetUniformValue1i(GetUniformLocation(name), value);
}

void
Shader::SetUniformValue (const std::string& name, float value)
{
    opengl::SetUniformValue1f(GetUniformLocation(name), value);
}

void
Shader::SetUniformValue (const std::string& name, uint32 count, int32* values)
{
    opengl::SetUniformValue1iv(GetUniformLocation(name), count, values);
}

void
Shader::SetUniformValue (const std::string& name, uint32 count, float* values)
{
    opengl::SetUniformValue1fv(GetUniformLocation(name), count, values);
}

void
Shader::SetUniformValue (const std::string& name, const math::vec2& vec)
{
    opengl::SetUniformValue2f(GetUniformLocation(name), vec.x, vec.y);
}

void
Shader::SetUniformValue (const std::string& name, const math::vec3& vec)
{
    opengl::SetUniformValue3f(GetUniformLocation(name), vec.x, vec.y, vec.z);
}

void
Shader::SetUniformValue (const std::string& name, const math::vec4& vec)
{
    opengl::SetUniformValue4f(GetUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
}

void
Shader::SetUniformValue (const std::string& name, const math::mat4& matrix)
{
    opengl::SetUniformValueMatrix4fv(GetUniformLocation(name), matrix.elements);
}

/* static */ Shader
Shader::FromFile (const char* restrict vert_path, const char* restrict frag_path)
{
    auto v = rdge::util::read_text_file(vert_path);
    auto f = rdge::util::read_text_file(frag_path);

    return Shader(v, f);
}

/* static */ uint32
Shader::MaxFragmentShaderUnits (void)
{
    static uint32 max_units = 0;

    if (max_units == 0)
    {
        max_units = opengl::GetInt(GL_MAX_TEXTURE_IMAGE_UNITS);
    }

    return max_units;
}

int32
Shader::GetUniformLocation (const std::string& name)
{
    auto it = m_uniforms.find(name);
    if (it != m_uniforms.end())
    {
        return it->second;
    }

    auto location = opengl::GetUniformLocation(m_programId, name);
    if (location >= 0)
    {
        m_uniforms.emplace(std::make_pair(name, location));
    }

    return location;
}

std::ostream& operator<< (std::ostream& os, ShaderType type)
{
    switch (type)
    {
    case ShaderType::Vertex:
        return os << "Vertex";
    case ShaderType::Fragment:
        return os << "Fragment";
    case ShaderType::Geometry:
        return os << "Geometry";
    default:
        break;
    }

    return os << "Unknown";
}

} // namespace rdge
