#include <rdge/graphics/shader.hpp>
#include <rdge/util/io.hpp>

#include <string>
#include <vector>

namespace RDGE {
namespace Graphics {

Shader::Shader (
                const char* restrict vertex_path,
                const char* restrict fragment_path
               )
{
    m_shaderId = glCreateProgram();
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vert_source = RDGE::Util::read_text_file(vertex_path);
    std::string frag_source = RDGE::Util::read_text_file(fragment_path);

    const char* c_vert_source = vert_source.c_str();
    const char* c_frag_source = frag_source.c_str();

    glShaderSource(vertex, 1, &c_vert_source, NULL);
    glCompileShader(vertex);

    GLint result;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
    if (UNLIKELY(result == GL_FALSE))
    {
        GLint length;
        glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> error(length);
        glGetShaderInfoLog(vertex, length, &length, &error[0]);
        glDeleteShader(vertex);

        // throw GLException
    }

    glShaderSource(fragment, 1, &c_frag_source, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
    if (UNLIKELY(result == GL_FALSE))
    {
        GLint length;
        glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> error(length);
        glGetShaderInfoLog(fragment, length, &length, &error[0]);
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        // throw GLException
    }

    glAttachShader(m_shaderId, vertex);
    glAttachShader(fragment, vertex);

    glLinkProgram(m_shaderId);
    glValidateProgram(m_shaderId);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader (void)
{
    glDeleteProgram(m_shaderId);
}

void
Shader::Enable (void) const
{
    glUseProgram(m_shaderId);
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

    return glGetUniformLocation(m_shaderId, name);
}

} // namespace Graphics
} // namespace RDGE
