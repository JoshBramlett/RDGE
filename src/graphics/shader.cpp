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

    std::string vert_source = RDGE::Util::read_file(vertex_path);
    std::string frag_source = RDGE::Util::read_file(fragment_path);

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
        std::vector<char> error { length };
        glGetShaderInfoLog(vertex, length, &length, &error[0]);
        glDeleteShader(vertex);

        // throw GLException
    }

    // TODO: Do same for fragment shader - remember to delete vertex shader also if failed

    glAttachShader(program, vertex);
    glAttachShader(fragment, vertex);

    glLinkProgram(program);
    glValidateProgram(program);

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
Shader::SetUniform1i (const GLchar* name, int value)
{
    // TODO: Should this be an int param?
    glUniform1f(getUniformLocation(name), value);
}

void
Shader::SetUniform2f (const GLchar* name, const maths::vec2& vec)
{
    glUniform2f(getUniformLocation(name), vec.x, vex.y);
}

void
Shader::SetUniform3f (const GLchar* name, const maths::vec3& vec)
{
    glUniform3f(getUniformLocation(name), vec.x, vex.y, vec.z);
}

void
Shader::SetUniform4f (const GLchar* name, const maths::vec4& vec)
{
    glUniform4f(getUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
}

void
Shader::SetUniformMat4 (const GLchar* name, const maths::mat4& matrix)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix.elements);
}

GLint
Shader::GetUniformLocation (const GLchar* name)
{
    // TODO: dipshit says this is slow, so we'll cache values in a later episode LUL

    glGetUniformLocation(m_shaderId, name);
}

} // namespace Graphics
} // namespace RDGE
