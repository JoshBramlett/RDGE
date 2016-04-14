//! \headerfile <rdge/graphics/shader.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/22/2016

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
    //! \brief Shader default ctor
    Shader (void);

    //! \brief Shader ctor
    //! \details Loads and compiles the shader source.  A program is created and linked,
    //!          and should be considered valid and ready for use.
    //! \param [in] vert_source GLSL source code for the vertex shader
    //! \param [in] frag_source GLSL source code for the fragment shader
    //! \throws RDGE::GLException Shader could not be built
    explicit Shader (const std::string& vert_source, const std::string& frag_source);

    //! \brief Shader dtor
    //! \details Deletes the program from OpenGL
    ~Shader (void);

    //! \brief Shader Copy ctor
    //! \details Non-copyable
    Shader (const Shader&) = delete;

    //! \brief Shader Move ctor
    //! \details Transfers ownership of the shader program
    Shader (Shader&& rhs) noexcept;

    //! \brief Shader Copy Assignment Operator
    //! \details Non-copyable
    Shader& operator= (const Shader&) = delete;

    //! \brief Shader Move Assignment Operator
    //! \details Transfers ownership of the shader program
    Shader& operator= (Shader&& rhs) noexcept;

    //! \brief Installs the program as part of current rendering state
    void Enable (void) const;

    //! \brief Uninstalls the program as part of current rendering state
    void Disable (void) const;

    // TODO Consider overloading
    void SetUniform1f (const GLchar* name, float value);
    void SetUniform2f (const GLchar* name, const RDGE::Math::vec2& vec);
    void SetUniform3f (const GLchar* name, const RDGE::Math::vec3& vec);
    void SetUniform4f (const GLchar* name, const RDGE::Math::vec4& vec);
    void SetUniformMat4 (const GLchar* name, const RDGE::Math::mat4& matrix);

    //! \brief Create a program from source files
    //! \details Performs all setup as defined in the constructor.
    //! \param [in] vert_path File path to GLSL source code for the vertex shader
    //! \param [in] frag_path File path to GLSL source code for the fragment shader
    //! \returns Initialized Shader object
    //! \throws RDGE::GLException Shader could not be built
    static Shader FromFile (const char* restrict vert_path, const char* restrict frag_path);

private:
    // TODO: Ideally this is where the parsing occurs in order to cache the uniform
    // locations, but I'm not sure if that data is available yet.  The sparky code
    // to cache uniforms is very convoluted, so we'll wait to finish this and keep
    // our naive method alone: GetUniformLocation
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
