//! \headerfile <rdge/graphics/shaders/sprite_batch_shader.hpp>
//! \author Josh Bramlett
//! \version 0.0.11
//! \date 03/12/2018

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/shaders/shader_program.hpp>
#include <rdge/math/vec4.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
struct color;
//!@}

//! \class SpriteBatchShader
//! \brief Shader program for use with the \ref SpriteBatch renderer
//! \brief Abstract class that forces uniformity of derived classes to conform
//!        to to format required for use with the \ref SpriteBatch renderer.
//!        Derived classes should override the necessary shader source methods,
//!        as well as the build method if any custom uniforms must be set.
class SpriteBatchShader
{
public:
    //!@{ SpriteBatchShader default ctor/dtor
    SpriteBatchShader (void) = default;
    virtual ~SpriteBatchShader (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    SpriteBatchShader (const SpriteBatchShader&) = delete;
    SpriteBatchShader& operator= (const SpriteBatchShader&) = delete;
    SpriteBatchShader (SpriteBatchShader&&) noexcept = default;
    SpriteBatchShader& operator= (SpriteBatchShader&&) noexcept = default;
    //!@}

    //! \brief Build the shader program
    virtual void Build (void);

protected:
    virtual const std::string& GetVertexSource (void);
    virtual const std::string& GetFragmentSource (void);

public:
    ShaderProgram shader;
};

//! \class DistanceFieldShader
//! \brief Shader program for rendering distance field textures
//! \see https://github.com/libgdx/libgdx/wiki/Distance-field-fonts
class DistanceFieldShader : public SpriteBatchShader
{
public:
    //!@{ DistanceFieldShader default ctor/dtor
    DistanceFieldShader (void) = default;
    ~DistanceFieldShader (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    DistanceFieldShader (const DistanceFieldShader&) = delete;
    DistanceFieldShader& operator= (const DistanceFieldShader&) = delete;
    DistanceFieldShader (DistanceFieldShader&&) noexcept = default;
    DistanceFieldShader& operator= (DistanceFieldShader&&) noexcept = default;
    //!@}

protected:
    //! \brief Custom DistanceFieldShader shader
    const std::string& GetFragmentSource (void) override;
};

//! \class DistanceFieldOutlineShader
//! \brief Shader program that adds an outline around a distance field texture
class DistanceFieldOutlineShader : public SpriteBatchShader
{
public:
    //!@{ Required shader uniforms
    static constexpr const char* U_OUTLINE_DISTANCE = "u_old"; //!< Outline distance
    static constexpr const char* U_OUTLINE_COLOR    = "u_olc"; //!< Outline color
    //!@}

    //!@{ Outline distance range
    static constexpr float MIN_DISTANCE = 0.f;  //!< Large outline
    static constexpr float MAX_DISTANCE = 0.5f; //!< No outline
    //!@}

    //! \brief DistanceFieldOutlineShader ctor
    //! \details Initializes the object with the provided values, but note the
    //!          shader will not be built until the Build() has been called.
    //! \param [in] c Outline color
    //! \param [in] d Outline distance (clamped to 0.f and 0.5f)
    //! \note The distance will be clamped to (MIN_DISTANCE, MAX_DISTANCE).
    DistanceFieldOutlineShader (const color& c, float d);

    //!@{ DistanceFieldOutlineShader default ctor/dtor
    DistanceFieldOutlineShader (void) = default;
    ~DistanceFieldOutlineShader (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    DistanceFieldOutlineShader (const DistanceFieldOutlineShader&) = delete;
    DistanceFieldOutlineShader& operator= (const DistanceFieldOutlineShader&) = delete;
    DistanceFieldOutlineShader (DistanceFieldOutlineShader&&) noexcept = default;
    DistanceFieldOutlineShader& operator= (DistanceFieldOutlineShader&&) noexcept = default;
    //!@}

    //! \brief Builds and passes the private parameters to the shader program
    void Build (void) override;

    //!@{
    //! \brief DistanceFieldOutlineShader uniform setters
    //! \details If the program has been not been built, the values will be cached.
    //!          Otherwise the values are forwarded to the shader.
    //! \note The distance will be clamped to (MIN_DISTANCE, MAX_DISTANCE).
    void SetOutlineColor (const color&);
    void SetOutlineDistance (float);
    //!@}

protected:
    //! \brief Custom DistanceFieldShader shader
    const std::string& GetFragmentSource (void) override;

private:
    math::vec4 m_color;
    float m_distance;
};

#if 0
class DistanceFieldDropShadowShader : public SpriteBatchShader
{
public:
    //!@{ DistanceFieldOutlineShader default ctor/dtor
    DistanceFieldDropShadowShader (void) = default;
    ~DistanceFieldDropShadowShader (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    DistanceFieldDropShadowShader (const DistanceFieldDropShadowShader&) = delete;
    DistanceFieldDropShadowShader& operator= (const DistanceFieldDropShadowShader&) = delete;
    DistanceFieldDropShadowShader (DistanceFieldDropShadowShader&&) noexcept = default;
    DistanceFieldDropShadowShader& operator= (DistanceFieldDropShadowShader&&) noexcept = default;
    //!@}

    //! \brief Build the shader program
    void Build (void) override;

    void SetShadowColor (const color& c);
    void SetShadowOffset (float offset);
    void SetShadowDistance (float distance);

protected:
    const std::string& GetFragmentSource (void) override;

private:
    math::vec4 m_color;
    float m_offset;
    float m_distance;
};
#endif

} // namespace rdge
