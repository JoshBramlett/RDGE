#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/opengl/wrapper.hpp>
#include <rdge/util/io.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>

namespace RDGE {
namespace Graphics {

namespace {

std::string ShaderTypeString (RDGE::UInt32 shader_type)
{
    switch (shader_type)
    {
    case GL_VERTEX_SHADER:
        return "vertex";
    case GL_FRAGMENT_SHADER:
        return "fragment";
    case GL_GEOMETRY_SHADER:
        return "geometry";
    default:
        break;
    }

    return "unknown";
}

} // anonymous namespace

Shader::Shader (
                const std::string& vert_source,
                const std::string& frag_source
               )
{
    std::vector<RDGE::UInt32> shaders;

    shaders.emplace_back(Compile(GL_VERTEX_SHADER, vert_source));
    shaders.emplace_back(Compile(GL_FRAGMENT_SHADER, frag_source));

    m_programId = Link(shaders);
}

/* static */ Shader
Shader::FromFile (const char* restrict vert_path, const char* restrict frag_path)
{
    auto v = RDGE::Util::read_text_file(vert_path);
    auto f = RDGE::Util::read_text_file(frag_path);

    return Shader(v, f);
}

Shader::~Shader (void)
{
    glDeleteProgram(m_programId);
}

void
Shader::Enable (void) const
{
    glUseProgram(m_programId);
}

void
Shader::Disable (void) const
{
    glUseProgram(0);
}

void
Shader::SetUniform1f (const GLchar* name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

void
Shader::SetUniform2f (const GLchar* name, const RDGE::Math::vec2& vec)
{
    glUniform2f(GetUniformLocation(name), vec.x, vec.y);
}

void
Shader::SetUniform3f (const GLchar* name, const RDGE::Math::vec3& vec)
{
    glUniform3f(GetUniformLocation(name), vec.x, vec.y, vec.z);
}

void
Shader::SetUniform4f (const GLchar* name, const RDGE::Math::vec4& vec)
{
    glUniform4f(GetUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
}

void
Shader::SetUniformMat4 (const GLchar* name, const RDGE::Math::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix.elements);
}

GLint
Shader::GetUniformLocation (const GLchar* name)
{
    // TODO: this is slow, so we'll cache values in a later episode LUL

    return glGetUniformLocation(m_programId, name);
}

void
Shader::PreProcess (void)
{

}

RDGE::UInt32
Shader::Compile (RDGE::UInt32 shader_type, const std::string& source)
{
    const char* src = source.c_str();

    RDGE::UInt32 shader = OpenGL::CreateShader(shader_type);

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
           << "type=" << ShaderTypeString(shader_type)
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

} // namespace Graphics
} // namespace RDGE
