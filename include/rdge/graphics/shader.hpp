//! \headerfile <rdge/graphics/shader.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 03/22/2016
//! \bug

#pragma once

#include <rdge/types.hpp>

#include <GL/glew.h>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

class Shader
{
public:
    explicit Shader (
                     const char* restrict vertex_path,
                     const char* restrict fragment_path
                    );

    ~Shader (void);

    void Enable (void) const;

    void Disable (void) const;

    void SetUniform1i (const GLchar* name, int value);
    void SetUniform2f (const GLchar* name, const maths::vec2& vec);
    void SetUniform3f (const GLchar* name, const maths::vec3& vec);
    void SetUniform4f (const GLchar* name, const maths::vec4& vec);
    void SetUniformMat4 (const GLchar* name, const maths::mat4& matrix);

private:
    GLuint GetUniformLocation (const GLchar* name);

    GLuint m_shaderId;
};

/* Example setunifrorm call:
 *
 * mat4 ortho = mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f);
 *
 * shader.enable();
 * shader.setUniformMat4("pr_matrix", ortho);
 * shader.setUniformMat4("ml_matrix", mat4::translation(vec3(4, 3, 0)));
 *
 * shader.setUniform2f("light_pos", vec2(4.0f, 1.5f));
 * shader.setUniform4f("colour", vec4(0.2f, 0.3f, 0.8f, 1.0f));
 *
 */

/* vert shader code from sparky
 *
 * #version 330 core
 *
 * layout (location = 0) in vec4 position;
 *
 * uniform mat4 pr_matrix;
 * uniform mat4 vw_matrix = mat4(1.0);
 * uniform mat4 ml_matrix = mat4(1.0);
 *
 * out vec4 pos;
 *
 * void main()
 * {
 *     gl_Position = pr_matrix * vw_matrix * ml_matrix * position;
 *     pos = position;
 * }
 */

/* frag shader code from sparky
 *
 * #version 330 core
 *
 * layout (location = 0) out vec4 color;
 *
 * uniform vec4 colour;
 * uniform vec4 light_pos;
 *
 * int vec4 pos;
 *
 * void main()
 * {
 *     float intensity = 1.0f / length(pos.xy - light_pos);
 *     color = colour * intensity;
 * }
 *
 */

} // namespace Graphics
} // namespace RDGE
