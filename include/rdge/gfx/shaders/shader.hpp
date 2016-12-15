//! \headerfile <rdge/gfx/shaders/shader.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 03/22/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/vec4.hpp>
#include <rdge/math/mat4.hpp>

#include <GL/glew.h>

#include <string>
#include <vector>
#include <unordered_map>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gfx {

enum class ShaderType : rdge::uint32
{
    Vertex   = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Geometry = GL_GEOMETRY_SHADER
};

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

    //! \brief Set the integer uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, rdge::int32 value);

    //! \brief Set the float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, float value);

    //! \brief Set the vec2 float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, const rdge::math::vec2& vec);

    //! \brief Set the vec3 float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, const rdge::math::vec3& vec);

    //! \brief Set the vec4 float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, const rdge::math::vec4& vec);

    //! \brief Set the integer array uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] count Number of elements to be modified
    //! \param [in] values Array of values to update
    void SetUniformValue (const std::string& name, rdge::uint32 count, rdge::int32* values);

    //! \brief Set the float array uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] count Number of elements to be modified
    //! \param [in] values Array of values to update
    void SetUniformValue (const std::string& name, rdge::uint32 count, float* values);

    //! \brief Set the mat4 float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] matrix Matrix to update
    void SetUniformValue (const std::string& name, const rdge::math::mat4& matrix);

    //! \brief Create a program from source files
    //! \details Performs all setup as defined in the constructor.
    //! \param [in] vert_path File path to GLSL source code for the vertex shader
    //! \param [in] frag_path File path to GLSL source code for the fragment shader
    //! \returns Initialized Shader object
    //! \throws RDGE::GLException Shader could not be built
    static Shader FromFile (const char* restrict vert_path, const char* restrict frag_path);

    //! \brief Create a program pre-defined for the SpriteBatch renderer
    //! \details Performs all setup as defined in the constructor.
    //! \returns Initialized Shader object
    //! \throws RDGE::GLException Shader could not be built
    static std::unique_ptr<Shader> SpriteBatch (void);

    //! \brief Number of textures supported in the fragment shader
    //! \details Queries OpenGL for the maximum amount of texture image units the
    //!          sampler in the fragment shader can access.  The minimum required
    //!          as defined by OpenGL is 16.
    //! \returns Number of supported textures in the fragment shader
    static rdge::uint32 MaxFragmentShaderUnits (void);

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
    rdge::uint32 Compile (ShaderType shader_type, const std::string& source);

    //! \brief Create and link program object
    //! \details Creates a program, attaches the provided shaders and links
    //!          the program.  Shaders are detached and deleted upon success.
    //! \param [in] shaders Collection of shader handles to attach
    //! \returns Unique program handle
    //! \throws RDGE::GLException Program could not be linked
    rdge::uint32 Link (const std::vector<rdge::uint32>& shaders);

    //! \brief Get the uniform location by name
    //! \details Uniform locations are unknown until the program is linked and
    //!          the query to OpenGL is slow so values are cached once the first
    //!          lookup is performed
    //! /param [in] name Uniform name
    //! /returns Shader program uniform location
    rdge::int32 GetUniformLocation (const std::string& name);

private:
    rdge::uint32 m_programId;

    std::unordered_map<std::string, rdge::int32> m_uniforms;
};

} // namespace gfx
} // namespace rdge

//! \brief ShaderType stream output operator
//! \param [in] os Output stream
//! \param [in] type ShaderType to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, rdge::gfx::ShaderType type);
