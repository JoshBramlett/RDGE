//! \headerfile <rdge/internal/opengl_wrapper.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/exception.hpp>

#include <GL/glew.h>
#include <cstddef>
#include <vector>
#include <array>
#include <type_traits>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace opengl {

// Verify OpenGL / RDGE type compatibility
// TODO Move to cpp file, so it'll get built
//      Is the above actually true?
static_assert(std::is_same<GLubyte,   uint8>::value,  "GLubyte != uint8");
static_assert(std::is_same<GLbyte,    int8>::value,   "GLbyte != int8");
static_assert(std::is_same<GLushort,  uint16>::value, "GLushort != uint16");
static_assert(std::is_same<GLshort,   int16>::value,  "GLshort != int16");
static_assert(std::is_same<GLuint,    uint32>::value, "GLuint != uint32");
static_assert(std::is_same<GLint,     int32>::value,  "GLint != int32");
static_assert(std::is_same<GLsizei,   int32>::value,  "GLsizei != int32");
static_assert(std::is_same<GLboolean, uint8>::value,  "GLboolean != uint8");
static_assert(std::is_same<GLenum,    uint32>::value, "GLenum != uint32");

static_assert(std::is_same<GLfloat, float>::value, "GLfloat != float");
static_assert(GL_FALSE == false, "GL_FALSE != false");
static_assert(GL_TRUE == true, "GL_TRUE != true");

//! \brief Throw exception if OpenGL error flag is set
//! \param [in] func OpenGL function name
//! \throws rdge::GLException with corresponding error code
inline void
gl_throw_on_error (const char* func)
{
    GLenum code = glGetError();
    if (code != GL_NO_ERROR)
    {
        GL_THROW("OpenGL call failed", func, static_cast<uint32>(code));
    }
}

constexpr GLboolean
to_glbool (bool value) { return value ? GL_TRUE : GL_FALSE; }

#ifdef RDGE_DEBUG
#define GL_CHECK_ERROR(x) \
do { \
    x; \
    gl_throw_on_error(#x); \
} while (false)
#else
#define GL_CHECK_ERROR(x) x
#endif

inline std::string
ErrorCodeToString (uint32 code)
{
#define GL_ERROR_STRING(e) case e: return #e;
    switch(code)
    {
    GL_ERROR_STRING(GL_NO_ERROR)
    GL_ERROR_STRING(GL_INVALID_ENUM)
    GL_ERROR_STRING(GL_INVALID_VALUE)
    GL_ERROR_STRING(GL_INVALID_OPERATION)
    GL_ERROR_STRING(GL_STACK_OVERFLOW)
    GL_ERROR_STRING(GL_STACK_UNDERFLOW)
    GL_ERROR_STRING(GL_OUT_OF_MEMORY)
    GL_ERROR_STRING(GL_INVALID_FRAMEBUFFER_OPERATION)
    GL_ERROR_STRING(GL_CONTEXT_LOST)
    GL_ERROR_STRING(GL_TABLE_TOO_LARGE)
    default:
        break;
    }
#undef GL_ERROR_STRING

    return "UNKNOWN (" + std::to_string(code) + ")";
}

/******************************************************************
 *                      OpenGL Get Values
 *****************************************************************/

// TODO: glGet* takes arrays as params, so this may need refactoring
//       depending on use

//! \brief Query OpenGL for the boolean value of a given parameter
//! \param [in] pname Parameter name
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glGet.xml
inline bool
GetBool (uint32 pname)
{
    GLboolean value;
    GL_CHECK_ERROR(glGetBooleanv(pname, &value));
    return (value == GL_TRUE);
}

//! \brief Query OpenGL for the float value of a given parameter
//! \param [in] pname Parameter name
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glGet.xml
inline float
GetFloat (uint32 pname)
{
    GLfloat value;
    GL_CHECK_ERROR(glGetFloatv(pname, &value));
    return value;
}

//! \brief Query OpenGL for the integer value of a given parameter
//! \param [in] pname Parameter name
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glGet.xml
inline int32
GetInt (uint32 pname)
{
    GLint value;
    GL_CHECK_ERROR(glGetIntegerv(pname, &value));
    return static_cast<int32>(value);
}

//! \brief Query OpenGL for the string value of a given parameter
//! \param [in] name Parameter name
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetString.xml
inline std::string
GetString (uint32 name)
{
    const unsigned char* result;
    GL_CHECK_ERROR(result = glGetString(name));
    return reinterpret_cast<const char*>(result);
}

/******************************************************************
 *                      Line Rasterization
 *****************************************************************/

//! \brief Get the supported line width max and min values
//! \details Queries via glGet using GL_ALIASED_LINE_WIDTH_RANGE.
//! \returns Array of floats including min and max values
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glGet.xml
inline std::array<float, 2>
GetSupportedLineWidth (void)
{
    std::array<float, 2> result;
    GL_CHECK_ERROR(glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, result.data()));
    return result;
}

//! \brief Specify the width of rasterized lines
//! \param [in] width Line width
//! \warning OpenGL spec does not require vendors support line width
//! \see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glLineWidth.xhtml
inline void
SetLineWidth (float width)
{
    GL_CHECK_ERROR(glLineWidth(width));
}

/******************************************************************
 *                       Enable/Disable
 *****************************************************************/

//! \brief Enable OpenGL capability
//! \param [in] cap Capability
//! \see https://www.opengl.org/sdk/docs/man4/docbook4/xhtml/glEnable.xml
inline void
Enable (uint32 cap)
{
    GL_CHECK_ERROR(glEnable(cap));
}

inline void
Disable (uint32 cap)
{
    GL_CHECK_ERROR(glDisable(cap));
}

/******************************************************************
 *                          Blending
 *****************************************************************/

//! \brief Set blending functions
//! \param [in] src Source factor
//! \param [in] dst Destinaction factor
//! \see https://www.opengl.org/sdk/docs/man4/html/glBlendFunc.xhtml
inline void
SetBlendFunction (uint32 src, uint32 dst)
{
    GL_CHECK_ERROR(glBlendFunc(src, dst));
}

//! \brief Set separate blending functions for RGB and alpha components
//! \param [in] src_rgb Source RGB factor
//! \param [in] dst_rgb Destinaction RGB factor
//! \param [in] src_alpha Source alpha factor
//! \param [in] dst_alpha Destinaction alpha factor
//! \see https://www.opengl.org/sdk/docs/man4/html/glBlendFuncSeparate.xhtml
inline void
SetBlendFunction (uint32 src_rgb, uint32 dst_rgb, uint32 src_alpha, uint32 dst_alpha)
{
    GL_CHECK_ERROR(glBlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha));
}

//! \brief Set blending equation
//! \param [in] mode How source and destination colors are combined
//! \see https://www.opengl.org/sdk/docs/man4/html/glBlendEquation.xhtml
inline void
SetBlendEquation (uint32 mode)
{
    GL_CHECK_ERROR(glBlendEquation(mode));
}

//! \brief Set separate blending equations for RGB and alpha components
//! \param [in] mode_rgb RGB blend equation
//! \param [in] mode_alpha Alpha blend equation
//! \see https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glBlendEquationSeparate.xml
inline void
SetBlendEquation (uint32 mode_rgb, uint32 mode_alpha)
{
    GL_CHECK_ERROR(glBlendEquationSeparate(mode_rgb, mode_alpha));
}

/******************************************************************
 *                          Shaders
 *****************************************************************/

//! \brief Direct map to glCreateShader
//! \details Creates an empty shader object.
//! \param [in] shader_type Type of shader to be created
//! \returns Unique non-zero identifier
//! \see https://www.opengl.org/sdk/docs/man/html/glCreateShader.xhtml
inline uint32
CreateShader (uint32 shader_type)
{
    uint32 handle;
    GL_CHECK_ERROR(handle = glCreateShader(shader_type));
    return handle;
}

//! \brief Set the shader source for a single shader
//! \details Replaces source code in a shader object.  Wrapper assumes source is
//!          a null terminated string.
//! \param [in] shader Handle of the shader
//! \param [in] source Source code to be loaded into the shader
//! \see https://www.opengl.org/sdk/docs/man/html/glShaderSource.xhtml
inline void
SetShaderSource (uint32 shader, const char** source)
{
    GL_CHECK_ERROR(glShaderSource(shader, 1, source, nullptr));
}

//! \brief Direct map to glCompileShader
//! \details Compile a shader object.
//! \param [in] shader Handle of the shader
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glCompileShader.xml
inline void
CompileShader (uint32 shader)
{
    GL_CHECK_ERROR(glCompileShader(shader));
}

//! \brief Direct map to glDeleteShader
//! \details Free up memory allocated to the shader
//! \param [in] shader Handle of the shader
//! \see https://www.opengl.org/sdk/docs/man/html/glDeleteShader.xhtml
inline void
DeleteShader (uint32 shader)
{
    GL_CHECK_ERROR(glDeleteShader(shader));
}

//! \brief Direct map to glCreateProgram
//! \details Create a empty shader program object
//! \returns Unique non-zero identifier
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glCreateProgram.xml
inline uint32
CreateProgram (void)
{
    uint32 handle;
    GL_CHECK_ERROR(handle = glCreateProgram());
    return handle;
}

//! \brief Direct map to glAttachShader
//! \details Attach a shader to a program object.
//! \param [in] program Handle of the program
//! \param [in] shader Handle of the shader
//! \see https://www.opengl.org/sdk/docs/man/html/glAttachShader.xhtml
inline void
AttachShader (uint32 program, uint32 shader)
{
    GL_CHECK_ERROR(glAttachShader(program, shader));
}

//! \brief Direct map to glDetachShader
//! \details Detach a shader from a program object.
//! \param [in] program Handle of the program
//! \param [in] shader Handle of the shader
//! \see https://www.opengl.org/sdk/docs/man/html/glDetachShader.xhtml
inline void
DetachShader (uint32 program, uint32 shader)
{
    GL_CHECK_ERROR(glDetachShader(program, shader));
}

//! \brief Direct map to glLinkProgram
//! \details Links specified program.
//! \param [in] program Handle of the program
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glLinkProgram.xml
inline void
LinkProgram (uint32 program)
{
    GL_CHECK_ERROR(glLinkProgram(program));
}

//! \brief Direct map to glUseProgram
//! \details Installs the program as part of the current rendering state.
//! \param [in] program Handle of the program
//! \note If zero is provided the rendering state will refer to an invalid
//!       program, and will have undefined behavior.
//! \see https://www.opengl.org/sdk/docs/man/html/glUseProgram.xhtml
inline void
UseProgram (uint32 program)
{
    GL_CHECK_ERROR(glUseProgram(program));
}

//! \brief Direct map to glGetUniformLocation
//! \details Get the location of a uniform variable
//! \param [in] program Handle of the program
//! \param [in] name Name of the uniform variable
//! \returns Uniform variable location
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetUniformLocation.xml
inline int32
GetUniformLocation (uint32 program, const std::string& name)
{
    int32 result;
    GL_CHECK_ERROR(result = glGetUniformLocation(program, name.c_str()));
    return result;
}

//! \brief Direct map to glUniform1i
//! \details Set the single integer uniform value for the current program
//! \param [in] location Location of the uniform to be modified
//! \param [in] v0 Value to set
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
inline void
SetUniformValue1i (int32 location, int32 v0)
{
    GL_CHECK_ERROR(glUniform1i(location, v0));
}

//! \brief Direct map to glUniform1f
//! \details Set the single float uniform value for the current program
//! \param [in] location Location of the uniform to be modified
//! \param [in] v0 Value to set
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
inline void
SetUniformValue1f (int32 location, float v0)
{
    GL_CHECK_ERROR(glUniform1f(location, v0));
}

//! \brief Direct map to glUniform2f
//! \details Set the vec2 float uniform value for the current program
//! \param [in] location Location of the uniform to be modified
//! \param [in] v0 Vector x value to set
//! \param [in] v1 Vector y value to set
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
inline void
SetUniformValue2f (int32 location, float v0, float v1)
{
    GL_CHECK_ERROR(glUniform2f(location, v0, v1));
}

//! \brief Direct map to glUniform3f
//! \details Set the vec3 float uniform value for the current program
//! \param [in] location Location of the uniform to be modified
//! \param [in] v0 Vector x value to set
//! \param [in] v1 Vector y value to set
//! \param [in] v2 Vector z value to set
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
inline void
SetUniformValue3f (int32 location, float v0, float v1, float v2)
{
    GL_CHECK_ERROR(glUniform3f(location, v0, v1, v2));
}

//! \brief Direct map to glUniform4f
//! \details Set the vec4 float uniform value for the current program
//! \param [in] location Location of the uniform to be modified
//! \param [in] v0 Vector x value to set
//! \param [in] v1 Vector y value to set
//! \param [in] v2 Vector z value to set
//! \param [in] v3 Vector w value to set
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
inline void
SetUniformValue4f (int32 location, float v0, float v1, float v2, float v3)
{
    GL_CHECK_ERROR(glUniform4f(location, v0, v1, v2, v3));
}

//! \brief Direct map to glUniform1iv
//! \details Set the integer array uniform value for the current program
//! \param [in] location Location of the uniform to be modified
//! \param [in] count Number of elements to be modified
//! \param [in] value Pointer to an array of values to update
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
inline void
SetUniformValue1iv (int32 location, uint32 count, int32* value)
{
    GL_CHECK_ERROR(glUniform1iv(location, count, value));
}

//! \brief Direct map to glUniform1fv
//! \details Set the float array uniform value for the current program
//! \param [in] location Location of the uniform to be modified
//! \param [in] count Number of elements to be modified
//! \param [in] value Pointer to an array of values to update
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
inline void
SetUniformValue1fv (int32 location, uint32 count, float* value)
{
    GL_CHECK_ERROR(glUniform1fv(location, count, value));
}

//! \brief Modified map to glUniformMatrix4fv
//! \details Set the float mat4 uniform value for the current program
//! \param [in] location Location of the uniform to be modified
//! \param [in] value Pointer to an array of values to update
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
inline void
SetUniformValueMatrix4fv (int32 location, const float* value)
{
    GL_CHECK_ERROR(glUniformMatrix4fv(location, 1, GL_FALSE, value));
}

/******************************************************************
 *                       Vertex Arrays
 *****************************************************************/

//! \brief Create a single vertex array from glGenVertexArrays
//! \details Generates a single vertex array object
//! \returns Identifier of the generated vertex array
//! \see https://www.opengl.org/sdk/docs/man/html/glGenVertexArrays.xhtml
inline uint32
CreateVertexArray (void)
{
    uint32 name;
    GL_CHECK_ERROR(glGenVertexArrays(1, &name));
    return name;
}

//! \brief Direct map to glGenVertexArrays
//! \details Generates multiple vertex array object names
//! \param [in] n Number of vertex array names to generate
//! \param [out] arrays Array where generated names are stored
//! \see https://www.opengl.org/sdk/docs/man/html/glGenVertexArrays.xhtml
inline void
CreateVertexArrays (int32 n, uint32* arrays)
{
    GL_CHECK_ERROR(glGenVertexArrays(n, arrays));
}

//! \brief Direct map to glBindVertexArray
//! \details Bind a vertex array object
//! \param [in] name Name of the vertex array to bind
//! \see https://www.opengl.org/sdk/docs/man/html/glBindVertexArray.xhtml
inline void
BindVertexArray (uint32 name)
{
    GL_CHECK_ERROR(glBindVertexArray(name));
}

//! \brief Break the existing vertex array binding
//! \see https://www.opengl.org/sdk/docs/man/html/glBindVertexArray.xhtml
inline void
UnbindVertexArrays (void)
{
    GL_CHECK_ERROR(glBindVertexArray(0));
}

inline void FreeVertexArray(uint32 array)
{
    GL_CHECK_ERROR(glDeleteVertexArrays(1, &array));
}

inline void FreeVertexArrays(uint32 size, uint32* arrays)
{
    GL_CHECK_ERROR(glDeleteVertexArrays(size, arrays));
}

//! \brief Direct map to glEnableVertexAttribArray
//! \details Enables a generic vertex attribute array
//! \param [in] index Index (used by shaders) of the generic vertex attribute
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glEnableVertexAttribArray.xml
inline void
EnableVertexAttribute (uint32 index)
{
    GL_CHECK_ERROR(glEnableVertexAttribArray(index));
}

inline void DisableVertexAttribute(uint32 index)
{
    GL_CHECK_ERROR(glDisableVertexAttribArray(index));
}

//! \brief Direct map to glVertexAttribPointer
//! \details Defines an array of generic vertex attribute data.  Essentially this
//!          tells OpenGL how to interpret the data within the buffer.
//! \param [in] index Index (used by shaders) of the generic vertex attribute
//! \param [in] size Number of components per attribute (3 for vec3, etc.)
//! \param [in] type Data type of each component in the array
//! \param [in] normalized True if integer formats should be mapped to floating point
//! \param [in] stride Byte offset between consecutive generic vertex attributes
//! \param [in] offset Offset of the first component of the first attribute
//! \see https://www.opengl.org/sdk/docs/man/html/glVertexAttribPointer.xhtml
inline void
VertexAttribPointer (uint32 index,
                     int32  size,
                     uint32 type,
                     bool   normalized,
                     uint32 stride,
                     void*  offset)
{
    GL_CHECK_ERROR(glVertexAttribPointer(index, size, type, to_glbool(normalized), stride, offset));
}

inline void
VertexAttribIPointer (uint32 index,
                      int32  size,
                      uint32 type,
                      uint32 stride,
                      void*  offset)
{
    GL_CHECK_ERROR(glVertexAttribIPointer(index, size, type, stride, offset));
}

// TODO glVertexAttribLPointer missing - used for doubles

//! \brief Direct map to glDrawElements
//! \details Renders primitives from array data.
//! \param [in] mode Type of primitive to render
//! \param [in] count The number of elements to be rendered
//! \param [in] type The type of values in indices parameter
//! \param [in] indices Pointer to indices storage
//! \see https://www.opengl.org/sdk/docs/man/html/glDrawElements.xhtml
inline void
DrawElements (uint32 mode, uint32 count, uint32 type, const void* indices)
{
    GL_CHECK_ERROR(glDrawElements(mode, count, type, indices));
}

//! \brief Direct map to glDrawArrays
//! \details Renders primitives from array data.
//! \param [in] mode Type of primitive to render
//! \param [in] first Starting index in the enabled arrays
//! \param [in] count The number of elements to be rendered
//! \see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
inline void
DrawArrays (uint32 mode, int32 first, uint32 count)
{
    GL_CHECK_ERROR(glDrawArrays(mode, first, count));
}

/******************************************************************
 *                           Buffers
 *****************************************************************/

//! \brief Create a single buffer from glGenBuffers
//! \details Generates a single buffer object
//! \returns Identifier of the generated buffer
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGenBuffers.xml
inline uint32
CreateBuffer (void)
{
    uint32 name;
    GL_CHECK_ERROR(glGenBuffers(1, &name));
    return name;
}

//! \brief Direct map to glGenBuffers
//! \details Generates multiple buffer object names
//! \param [in] n Number of buffer names to generate
//! \param [out] buffers Array where generated names are stored
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGenBuffers.xml
inline void
CreateBuffers (int32 n, uint32* buffers)
{
    GL_CHECK_ERROR(glGenBuffers(n, buffers));
}

//! \brief Direct map to glBindBuffer
//! \details Bind the buffer object to the binding target
//! \param [in] target The target to which the buffer is bound
//! \param [in] buffer Identifier of the buffer object
//! \see https://www.khronos.org/opengles/sdk/1.1/docs/man/glBindBuffer.xml
inline void
BindBuffer (uint32 target, uint32 buffer)
{
    GL_CHECK_ERROR(glBindBuffer(target, buffer));
}

//! \brief Break all existing buffer bindings for the target
//! \param [in] target The target to which the buffer will be unbound
//! \see https://www.khronos.org/opengles/sdk/1.1/docs/man/glBindBuffer.xml
inline void
UnbindBuffers (uint32 target)
{
    GL_CHECK_ERROR(glBindBuffer(target, 0));
}

//! \brief Delete the existing buffer with the provided name
//! \param [in] name Name of the buffer
//! \see https://www.opengl.org/sdk/docs/man/html/glDeleteBuffers.xhtml
inline void
DeleteBuffer (uint32 name)
{
    GL_CHECK_ERROR(glDeleteBuffers(1, &name));
}

//! \brief Direct map to glDeleteBuffers
//! \details Deletes n buffer objects named by the elements in the array.
//! \param [in] n Number of buffers to delete
//! \param [in] buffers Array of buffer objects to be deleted
//! \see https://www.opengl.org/sdk/docs/man/html/glDeleteBuffers.xhtml
inline void
DeleteBuffers (uint32 n, uint32* buffers)
{
    GL_CHECK_ERROR(glDeleteBuffers(n, buffers));
}

//! \brief Direct map to glBufferData
//! \details Creates and initializes a buffer object's data store
//! \param [in] target The target to which the buffer is bound
//! \param [in] size Size in bytes of the data store
//! \param [in] data Pointer to the data to be copied to the data store
//! \param [in] usage Expected usage pattern (e.g. Read, Write, etc.)
//! \see https://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml
inline void
SetBufferData (uint32 target, std::ptrdiff_t size, const void* data, uint32 usage)
{
    GL_CHECK_ERROR(glBufferData(target, size, data, usage));
}

//! \brief Direct map to glMapBuffer
//! \details Maps to the clients address space the entire data store of the buffer
//!          bound to by the provided target.
//! \param [in] target The target buffer object being mapped
//! \param [in] access Access policy (read, write, read/write)
//! \returns Pointer to the data store
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glMapBuffer.xml
inline void*
GetBufferPointer (uint32 target, uint32 access)
{
    void* result = nullptr;
    GL_CHECK_ERROR(result = glMapBuffer(target, access));
    return result;
}

//! \brief Direct map to glUnmapBuffer
//! \details Releases the mapping and invalidates the pointer to the data store.
//!          Unmapping is required before the buffer object can be used.
//! \param [in] target The target buffer object being unmapped
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glMapBuffer.xml
inline bool
ReleaseBufferPointer (uint32 target)
{
    uint32 result;
    GL_CHECK_ERROR(result = glUnmapBuffer(target));
    return result == GL_TRUE;
}



/******************************************************************
 *                         Frame Buffers
 *****************************************************************/

inline uint32 CreateFrameBuffer()
{
    uint32 name;
    GL_CHECK_ERROR(glGenFramebuffers(1, &name));
    return name;
}

inline void CreateFramebuffers(uint32 size, uint32* buffers)
{
    GL_CHECK_ERROR(glGenFramebuffers(size, buffers));
}

inline void BindFramebuffer(uint32 target, uint32 buffer)
{
    GL_CHECK_ERROR(glBindFramebuffer(target, buffer));
}

inline void FreeFramebuffer(uint32 buffer)
{
    GL_CHECK_ERROR(glDeleteFramebuffers(1, &buffer));
}

inline void FreeFramebuffers(uint32 size, uint32* buffers)
{
    GL_CHECK_ERROR(glDeleteFramebuffers(size, buffers));
}

/******************************************************************
 *                         Render Buffers
 *****************************************************************/

inline uint32 CreateRenderBuffer()
{
    uint32 result;
    GL_CHECK_ERROR(glGenRenderbuffers(1, &result));
    return result;
}

inline void CreateRenderbuffers(uint32 size, uint32* buffers)
{
    GL_CHECK_ERROR(glGenRenderbuffers(size, buffers));
}

inline void BindRenderbuffer(uint32 target, uint32 buffer)
{
    GL_CHECK_ERROR(glBindRenderbuffer(target, buffer));
}

/******************************************************************
 *                          Textures
 *****************************************************************/

//! \brief Create a single texture from glGenTextures
//! \details Generates a single texture object
//! \returns Identifier of the generated texture
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGenTextures.xml
inline uint32
CreateTexture (void)
{
    uint32 name;
    GL_CHECK_ERROR(glGenTextures(1, &name));
    return name;
}

//! \brief Direct map to glGenTextures
//! \details Generates multiple texture object names
//! \param [in] n Number of texture names to generate
//! \param [out] textures Array where generated names are stored
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGenTextures.xml
inline void
CreateTextures (uint32 n, uint32* textures)
{
    // TODO: Be smart, return an array
    GL_CHECK_ERROR(glGenTextures(n, textures));
}

//! \brief Delete a single texture from glDeleteTextures
//! \details Deletes a single texture object
//! \see https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glDeleteTextures.xml
inline void
DeleteTexture (uint32 name)
{
    GL_CHECK_ERROR(glDeleteTextures(1, &name));
}

inline void DeleteTextures (uint32 size, uint32* textures)
{
    GL_CHECK_ERROR(glDeleteTextures(size, textures));
}

//! \brief Direct map to glBindTexture
//! \details Bind the texture object to the binding target
//! \param [in] target The target to which the texture is bound
//! \param [in] texture Identifier of the texture object
//! \see https://www.opengl.org/sdk/docs/man/html/glBindTexture.xhtml
inline void
BindTexture (uint32 target, uint32 texture)
{
    GL_CHECK_ERROR(glBindTexture(target, texture));
}

//! \brief Break all existing texture bindings for the target
//! \param [in] target The target to which the texture will be unbound
//! \see https://www.opengl.org/sdk/docs/man/html/glBindTexture.xhtml
inline void
UnbindTexture (uint32 target)
{
    GL_CHECK_ERROR(glBindTexture(target, 0));
}

//! \brief Direct map to glTexParameteri
//! \details Set texture parameters to the binding target
//! \param [in] target The target to which the texture is bound
//! \param [in] pname Parameter name
//! \param [in] param Parameter value
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
inline void
SetTextureParameter (uint32 target, uint32 pname, int32 param)
{
    GL_CHECK_ERROR(glTexParameteri(target, pname, param));
}

//! \brief Defines a texture image
//! \details Describes the parameters of the image, and how the texture will be
//!          stored in memory.
//! \param [in] target The target to which the texture is bound
//! \param [in] internalformat Internal format of the texture
//! \param [in] width Texture width
//! \param [in] height Texture height
//! \param [in] format Format of the texel data
//! \param [in] type Data type of the texel data
//! \param [in] data Pointer to the image data in memory
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexImage2D.xml
inline void
UploadTextureData (uint32 target,
                   int32  internalformat,
                   int32  width,
                   int32  height,
                   int32  format,
                   uint32 type,
                   const void* data)
{
    GL_CHECK_ERROR(glTexImage2D(target,
                                0,              // level-of-detail
                                internalformat,
                                width,
                                height,
                                0,              // width of the border.  must be 0.
                                format,
                                type,
                                data));
}

//! \brief Direct map to glActiveTexture
//! \details Set the active texture unit.
//! \param [in] texture Texture unit to activate
//! \see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glActiveTexture.xml
inline void
SetActiveTexture (uint32 texture)
{
    GL_CHECK_ERROR(glActiveTexture(texture));
}

/******************************************************************
 *                     OpenGL Render Phase
 *****************************************************************/

//! \brief Direct map to glClear
//! \details Clear buffer to preset values.
//! \param [in] mask Bitwise OR of masks that indicate the buffers to be cleared
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glClear.xml
inline void
Clear (uint32 mask)
{
    GL_CHECK_ERROR(glClear(mask));
}

//! \brief Direct map to glClearColor
//! \details Specify clear values for the color buffers.
//! \param [in] red Red value
//! \param [in] green Green value
//! \param [in] blue Blue value
//! \param [in] alpha Alpha value
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glClearColor.xml
inline void
SetClearColor (float red, float green, float blue, float alpha)
{
    GL_CHECK_ERROR(glClearColor(red, green, blue, alpha));
}

//! \brief Direct map to glViewport
//! \details Set the viewport.
//! \param [in] x lower left x-coordinate of the viewport
//! \param [in] y lower left y-coordinate of the viewport
//! \param [in] width Width of the viewport
//! \param [in] height Height of the viewport
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glViewport.xml
inline void
SetViewport (int32 x, int32 y, uint32 width, uint32 height)
{
    GL_CHECK_ERROR(glViewport(x, y, width, height));
}

//! \brief Get the current viewport
//! \details Queries via glGet using GL_VIEWPORT.
//! \returns Vector of floats including x, y, width and height
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glGet.xml
inline std::vector<float>
GetViewport (void)
{
    std::vector<float> viewport(4);
    GL_CHECK_ERROR(glGetFloatv(GL_VIEWPORT, viewport.data()));
    return viewport;
}

/******************************************************************
 *                          TODO
 *****************************************************************/

inline int GetScreenBuffer()
{
    int result;
    GL_CHECK_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result));
    return result;
}

inline void SetBufferSubData(uint32 target, ptrdiff_t offset, ptrdiff_t size, const void* data)
{
    GL_CHECK_ERROR(glBufferSubData(target, offset, size, data));
}

inline void FramebufferTexture2D(uint32 target, uint32 attachment, uint32 textarget, uint32 texture, int level)
{
    GL_CHECK_ERROR(glFramebufferTexture2D(target, attachment, textarget, texture, level));
}

inline void FramebufferRenderbuffer(uint32 target, uint32 attachment, uint32 renderbuffertarget, uint32 renderbuffer)
{
    GL_CHECK_ERROR(glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer));
}

inline void RenderbufferStorage(uint32 target, uint32 format, uint32 width, uint32 height)
{
    GL_CHECK_ERROR(glRenderbufferStorage(target, format, width, height));
}

} // namespace opengl
} // namespace rdge
