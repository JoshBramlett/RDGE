//! \headerfile <rdge/graphics/shader.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/22/2016
//! \bug

#pragma once

#include <rdge/types.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/vec4.hpp>
#include <rdge/math/mat4.hpp>

#include <GL/glew.h>

#include <string>
#include <vector>

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

    void SetUniform1f (const GLchar* name, float value);
    void SetUniform2f (const GLchar* name, const RDGE::Math::vec2& vec);
    void SetUniform3f (const GLchar* name, const RDGE::Math::vec3& vec);
    void SetUniform4f (const GLchar* name, const RDGE::Math::vec4& vec);
    void SetUniformMat4 (const GLchar* name, const RDGE::Math::mat4& matrix);

private:
    void PreProcess (void);

    //! \brief Compile shader source
    //! \details Creates a shader, and loads and compiles the source.
    //! \param [in] shader_type Type of shader to compile
    //! \param [in] source GLSL source code of the shader
    //! \returns Unique shader handle
    //! \throws RDGE::GLException Shader could not be compiled
    RDGE::UInt32 Compile (RDGE::UInt32 shader_type, const std::string& source);

    //! \brief Create and link program object
    //! \details Creates a program, attaches the provided shaders and links
    //!          the program.  Shaders are detached and deleted upon success.
    //! \param [in] shaders Collection of shader handles to attach
    //! \returns Unique program handle
    //! \throws RDGE::GLException Program could not be linked
    RDGE::UInt32 Link (const std::vector<RDGE::UInt32>& shaders);


    GLint GetUniformLocation (const GLchar* name);

    GLuint m_programId;
};

} // namespace Graphics
} // namespace RDGE
