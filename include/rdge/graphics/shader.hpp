//! \headerfile <rdge/graphics/shader.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/12/2016

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

enum class ShaderType : uint32
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
    //! \details Loads and compiles the shader source.
    //! \param [in] vert_source GLSL source code for the vertex shader
    //! \param [in] frag_source GLSL source code for the fragment shader
    //! \throws RDGE::GLException Shader could not be built
    explicit Shader (const std::string& vert_source, const std::string& frag_source);

    //! \brief Shader dtor
    //! \details Deletes the shader program from the OpenGL context
    ~Shader (void) noexcept;

    //!@{
    //! \brief Non-copyable, move enabled
    Shader (const Shader&) = delete;
    Shader& operator= (const Shader&) = delete;
    Shader (Shader&&) noexcept;
    Shader& operator= (Shader&&) noexcept;
    //!@}

    //! \brief Installs the program as part of current rendering state
    void Enable (void) const;

    //! \brief Uninstalls the program as part of current rendering state
    void Disable (void) const;

    //! \brief Set the integer uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, int32 value);

    //! \brief Set the float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, float value);

    //! \brief Set the vec2 float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, const math::vec2& vec);

    //! \brief Set the vec3 float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, const math::vec3& vec);

    //! \brief Set the vec4 float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] value Value to set the uniform to
    void SetUniformValue (const std::string& name, const math::vec4& vec);

    //! \brief Set the integer array uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] count Number of elements to be modified
    //! \param [in] values Array of values to update
    void SetUniformValue (const std::string& name, uint32 count, int32* values);

    //! \brief Set the float array uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] count Number of elements to be modified
    //! \param [in] values Array of values to update
    void SetUniformValue (const std::string& name, uint32 count, float* values);

    //! \brief Set the mat4 float uniform variable value
    //! \param [in] name Name of the uniform variable
    //! \param [in] matrix Matrix to update
    void SetUniformValue (const std::string& name, const math::mat4& matrix);

    //! \brief Create a program from source files
    //! \details Performs all setup as defined in the constructor.
    //! \param [in] vert_path File path to GLSL source code for the vertex shader
    //! \param [in] frag_path File path to GLSL source code for the fragment shader
    //! \returns Initialized Shader object
    //! \throws RDGE::GLException Shader could not be built
    static Shader FromFile (const char* restrict vert_path, const char* restrict frag_path);

    //! \brief Number of textures supported in the fragment shader
    //! \details Queries OpenGL for the maximum amount of texture image units the
    //!          sampler in the fragment shader can access.  The minimum required
    //!          as defined by OpenGL is 16.
    //! \returns Number of supported textures in the fragment shader
    static int32 MaxFragmentShaderUnits (void);

private:
    //! \brief Get the uniform location by name
    //! \details Uniform locations are unknown until the program is linked and
    //!          the query to OpenGL is slow so values are cached once the first
    //!          lookup is performed
    //! /param [in] name Uniform name
    //! /returns Shader program uniform location
    int32 GetUniformLocation (const std::string& name);

private:
    uint32 m_programId = 0; //!< OpenGL program handle
    std::unordered_map<std::string, int32> m_uniforms; //!< Cached list of uniform lookups

    // TODO m_uniforms is a great candidate for a custom data structure.  The number
    //      of uniforms is limited, so this map will be small.  Therefore some
    //      contiguous memory data structure could be used.
    //
    //      The only drawback being this suffers from the whole "premature optimization
    //      is the root of all evil" bullshit.  If I do this, it's for learning
    //      purposes, not for functionality.
};

//! \brief ShaderType stream output operator
//! \param [in] os Output stream
//! \param [in] type ShaderType to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, ShaderType type);

} // namespace rdge
