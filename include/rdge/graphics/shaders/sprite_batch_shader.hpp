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
    //!@{ Required shader uniforms
    static constexpr const char* U_SMOOTHING = "u_smoothing"; //!< Texture smoothing
    //!@}

    //! \brief DistanceFieldShader ctor
    //! \details Initializes the object with the provided values, but note the
    //!          shader will not be built until the Build() has been called.
    //!          The smoothing uniform is calculated by (0.25 / (spread * scale)).
    //! \param [in] spread Distance spread (used in texture generation)
    //! \param [in] scale Scale the texture is drawn at
    DistanceFieldShader (float spread, float scale);

    //!@{ DistanceFieldShader default ctor/dtor
    DistanceFieldShader (void) = default;
    virtual ~DistanceFieldShader (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    DistanceFieldShader (const DistanceFieldShader&) = delete;
    DistanceFieldShader& operator= (const DistanceFieldShader&) = delete;
    DistanceFieldShader (DistanceFieldShader&&) noexcept = default;
    DistanceFieldShader& operator= (DistanceFieldShader&&) noexcept = default;
    //!@}

    //! \brief Builds and passes the private parameters to the shader program
    void Build (void) override;

    //!@{
    //! \brief DistanceFieldOutlineShader uniform setters
    //! \details If the program has been not been built, the values will be cached.
    //!          Otherwise the values are forwarded to the shader.
    //! \note The distance will be clamped to (MIN_DISTANCE, MAX_DISTANCE).
    void SetSpread (float);
    void SetScale (float);
    //!@}

protected:
    //! \brief Custom DistanceFieldShader shader
    const std::string& GetFragmentSource (void) override;

private:
    float m_spread = 0.f;
    float m_scale = 0.f;
};

//! \class DistanceFieldOutlineShader
//! \brief Shader program that adds an outline around a distance field texture
class DistanceFieldOutlineShader : public DistanceFieldShader
{
public:
    //!@{ Required shader uniforms
    static constexpr const char* U_OUTLINE_COLOR    = "u_olc"; //!< Outline color
    static constexpr const char* U_OUTLINE_DISTANCE = "u_old"; //!< Outline distance
    //!@}

    //!@{ Outline distance range
    static constexpr float MIN_DISTANCE = 0.f;  //!< Large outline
    static constexpr float MAX_DISTANCE = 0.5f; //!< No outline
    //!@}

    //! \brief DistanceFieldOutlineShader ctor
    //! \details Initializes the object with the provided values, but note the
    //!          shader will not be built until the Build() has been called.
    //! \param [in] spread Distance spread (used in texture generation)
    //! \param [in] scale Scale the texture is drawn at
    //! \param [in] c Outline color
    //! \param [in] d Outline distance (clamped to 0.f and 0.5f)
    //! \note The distance will be clamped to (MIN_DISTANCE, MAX_DISTANCE).
    DistanceFieldOutlineShader (float spread, float scale, const color& c, float d);

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

//! \class DistanceFieldDropShadowShader
//! \brief Shader program that adds drap shadow around a distance field texture
class DistanceFieldDropShadowShader : public DistanceFieldShader
{
public:
    //!@{ Required shader uniforms
    static constexpr const char* U_SHADOW_COLOR     = "u_dsc"; //!< Shadow color
    static constexpr const char* U_SHADOW_SMOOTHING = "u_dss"; //!< Shadow smoothing
    static constexpr const char* U_SHADOW_OFFSET    = "u_dso"; //!< Shadow offset
    //!@}

    //!@{ Shadow smoothing range
    static constexpr float MIN_SMOOTHING = 0.f;  //!< Minimum shadow smoothing
    static constexpr float MAX_SMOOTHING = 0.5f; //!< Maximum shadow smoothing
    //!@}

    //! \brief DistanceFieldDropShadowShader ctor
    //! \details Initializes the object with the provided values, but note the
    //!          shader will not be built until the Build() has been called.
    //!          Smoothing (blur) on the drop shadow will be clamped to
    //!          (MIN_SMOOTHING, MAX_SMOOTHING), with higher values producing
    //!          more blur.  The sign of the offset values determine the location
    //!          of the drop shadow (e.g. -x,y is bottom-left).  Valid values
    //!          range from 0 to (distance field spread / texture width/height).
    //! \param [in] spread Distance spread (used in texture generation)
    //! \param [in] scale Scale the texture is drawn at
    //! \param [in] c Shadow color
    //! \param [in] s Shadow smoothing (clamped to 0.f and 0.5f)
    //! \param [in] offset Offset placement of the shadow
    //! \note Artifacts may be visible for values reaching the max smoothing and
    //!       offset.  Adjust to taste.
    DistanceFieldDropShadowShader (float spread,
                                   float scale,
                                   const color& c,
                                   float s,
                                   const math::vec2& offset);

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

    //! \brief Builds and passes the private parameters to the shader program
    void Build (void) override;

    //!@{
    //! \brief DistanceFieldDropShadowShader uniform setters
    //! \details If the program has been not been built, the values will be cached.
    //!          Otherwise the values are forwarded to the shader.
    //! \note The smoothing will be clamped to (MIN_SMOOTHING, MAX_SMOOTHING).
    //! \note Offset can be any value between 0.f and (distance field spread /
    //!       texture size).
    void SetShadowColor (const color&);
    void SetShadowSmoothing (float);
    void SetShadowOffset (const math::vec2&);
    //!@}

protected:
    //! \brief Custom DistanceFieldShader shader
    const std::string& GetFragmentSource (void) override;

private:
    math::vec4 m_color;
    float m_smoothing;
    math::vec2 m_offset;
};

} // namespace rdge
