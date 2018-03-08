//! \headerfile <rdge/graphics/shader.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/12/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/compiler.hpp>

#include <GL/glew.h>

#include <unordered_map>

//!@{ Forward declarations
namespace rdge {
namespace math {
struct vec3;
struct vec4;
struct mat4;
} // namespace math
} // namespace rdge
//!@}

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

enum class ShaderType : uint32
{
    VERTEX   = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER
};

class Shader
{
public:
    //! \brief Shader default ctor
    Shader (void) = default;

    //! \brief Shader ctor
    //! \details Loads and compiles the shader source.
    //! \param [in] vert_source GLSL source code for the vertex shader
    //! \param [in] frag_source GLSL source code for the fragment shader
    //! \throws RDGE::GLException Shader could not be built
    explicit Shader (const std::string& vert_source, const std::string& frag_source);

    //! \brief Shader dtor
    //! \details Deletes the shader program from the OpenGL context
    ~Shader (void) noexcept;

    //!@{ Non-copyable, move enabled
    Shader (const Shader&) = delete;
    Shader& operator= (const Shader&) = delete;
    Shader (Shader&&) noexcept;
    Shader& operator= (Shader&&) noexcept;
    //!@}

    //!@{ Install/uninstall the program as part of the current rendering state
    void Enable (void) const;
    void Disable (void) const;
    //!@}

    //!@{ Uniform setter overloads
    void SetUniformValue (const std::string& name, int32 value);
    void SetUniformValue (const std::string& name, float value);
    void SetUniformValue (const std::string& name, uint32 count, int32* values);
    void SetUniformValue (const std::string& name, uint32 count, float* values);
    void SetUniformValue (const std::string& name, const math::vec2& vec);
    void SetUniformValue (const std::string& name, const math::vec3& vec);
    void SetUniformValue (const std::string& name, const math::vec4& vec);
    void SetUniformValue (const std::string& name, const math::mat4& matrix);
    //!@}

    //! \brief Create a program from source files
    //! \details Performs all setup as defined in the constructor.
    //! \param [in] vert_path File path to GLSL source code for the vertex shader
    //! \param [in] frag_path File path to GLSL source code for the fragment shader
    //! \returns Initialized Shader object
    //! \throws rdge::GLException Shader could not be built
    static Shader FromFile (const char* restrict vert_path, const char* restrict frag_path);

    //! \brief Number of textures supported in the fragment shader
    //! \details Queries OpenGL for the maximum amount of texture image units the
    //!          sampler in the fragment shader can access.  The minimum required
    //!          as defined by OpenGL is 16.
    //! \returns Number of supported textures in the fragment shader
    static uint32 MaxFragmentShaderUnits (void);

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
std::ostream& operator<< (std::ostream&, ShaderType);

//!@{ ShaderType string conversions
std::string to_string (ShaderType);
bool try_parse (const std::string&, ShaderType);
//!@}

} // namespace rdge
