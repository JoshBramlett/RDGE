#include <rdge/graphics/shaders/shader.hpp>
#include <rdge/util/io.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include "spritebatch.shader.hpp"

#include <GL/glew.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>

namespace RDGE {
namespace Graphics {

namespace {

    // cache value for multiple lookups
    RDGE::Int32 s_maxFragmentShaderUnits = -1;

} // anonymous namespace

Shader::Shader (void)
    : m_programId(0)
{ }

Shader::Shader (const std::string& vert_source, const std::string& frag_source)
    : m_programId(0)
{
    std::vector<RDGE::UInt32> shaders;

    shaders.emplace_back(Compile(ShaderType::Vertex, vert_source));
    shaders.emplace_back(Compile(ShaderType::Fragment, frag_source));

    m_programId = Link(shaders);
}

Shader::~Shader (void)
{
    glDeleteProgram(m_programId);
}

Shader::Shader (Shader&& rhs) noexcept
    : m_uniforms(std::move(rhs.m_uniforms))
{
    // The destructor deletes the OpenGL program, therefore we swap the program
    // ids so the moved-from object will destroy the program it's replacing
    std::swap(m_programId, rhs.m_programId);
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
    OpenGL::UseProgram(m_programId);
}

void
Shader::Disable (void) const
{
    OpenGL::UseProgram(0);
}

void
Shader::SetUniformValue (const std::string& name, RDGE::Int32 value)
{
    OpenGL::SetUniformValue1i(GetUniformLocation(name), value);
}

void
Shader::SetUniformValue (const std::string& name, float value)
{
    OpenGL::SetUniformValue1f(GetUniformLocation(name), value);
}

void
Shader::SetUniformValue (const std::string& name, const RDGE::Math::vec2& vec)
{
    OpenGL::SetUniformValue2f(GetUniformLocation(name), vec.x, vec.y);
}

void
Shader::SetUniformValue (const std::string& name, const RDGE::Math::vec3& vec)
{
    OpenGL::SetUniformValue3f(GetUniformLocation(name), vec.x, vec.y, vec.z);
}

void
Shader::SetUniformValue (const std::string& name, const RDGE::Math::vec4& vec)
{
    OpenGL::SetUniformValue4f(GetUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
}

void
Shader::SetUniformValue (const std::string& name, RDGE::UInt32 count, RDGE::Int32* values)
{
    OpenGL::SetUniformValue1iv(GetUniformLocation(name), count, values);
}

void
Shader::SetUniformValue (const std::string& name, RDGE::UInt32 count, float* values)
{
    OpenGL::SetUniformValue1fv(GetUniformLocation(name), count, values);
}

void
Shader::SetUniformValue (const std::string& name, const RDGE::Math::mat4& matrix)
{
    OpenGL::SetUniformValueMatrix4fv(GetUniformLocation(name), matrix.elements);
}

/* static */ Shader
Shader::FromFile (const char* restrict vert_path, const char* restrict frag_path)
{
    auto v = RDGE::Util::read_text_file(vert_path);
    auto f = RDGE::Util::read_text_file(frag_path);

    return Shader(v, f);
}

/* static */ std::unique_ptr<Shader>
Shader::SpriteBatch (void)
{
    return std::make_unique<Shader>(
                                    SpriteBatchVertexShaderSource(),
                                    SpriteBatchFragmentShaderSource()
                                   );
}

/* static */ RDGE::Int32
Shader::MaxFragmentShaderUnits (void)
{
    if (s_maxFragmentShaderUnits < 0)
    {
        s_maxFragmentShaderUnits = OpenGL::GetIntegerValue(GL_MAX_TEXTURE_IMAGE_UNITS);
    }

    return s_maxFragmentShaderUnits;
}

void
Shader::PreProcess (void)
{
    // TODO: Implement
}

RDGE::UInt32
Shader::Compile (ShaderType shader_type, const std::string& source)
{
    const char* src = source.c_str();

    RDGE::UInt32 shader = OpenGL::CreateShader(static_cast<RDGE::UInt32>(shader_type));

    OpenGL::SetShaderSource(shader, &src);
    OpenGL::CompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (UNLIKELY(status == GL_FALSE))
    {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> error(length);
        glGetShaderInfoLog(shader, length, &length, &error[0]);
        glDeleteShader(shader);

        std::stringstream ss;
        ss << "Shader compilation failed. "
           << "type=" << shader_type
           << "info=" << error.data();

        GL_THROW(ss.str(), "", 0);
    }

    return shader;
}

RDGE::UInt32
Shader::Link (const std::vector<RDGE::UInt32>& shaders)
{
    RDGE::UInt32 program = OpenGL::CreateProgram();

    for (auto shader : shaders)
    {
        OpenGL::AttachShader(program, shader);
    }

    OpenGL::LinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (UNLIKELY(status == GL_FALSE))
    {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> error(length);
        glGetProgramInfoLog(program, length, &length, &error[0]);

        std::stringstream ss;
        ss << "Program linking failed. "
           << "info=" << error.data();

        GL_THROW(ss.str(), "", 0);
    }

    for (auto shader : shaders)
    {
        OpenGL::DetachShader(program, shader);
        OpenGL::DeleteShader(shader);
    }

    return program;
}

RDGE::Int32
Shader::GetUniformLocation (const std::string& name)
{
    auto it = m_uniforms.find(name);
    if (it != m_uniforms.end())
    {
        return it->second;
    }

    auto location = OpenGL::GetUniformLocation(m_programId, name);
    if (location >= 0)
    {
        m_uniforms.emplace(std::make_pair(name, location));
    }

    return location;
}

std::ostream& operator<< (std::ostream& os, ShaderType shader_type)
{
    switch (shader_type)
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

} // namespace Graphics
} // namespace RDGE
