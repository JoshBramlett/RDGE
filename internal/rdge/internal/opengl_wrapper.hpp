//! \headerfile <rdge/internal/opengl_wrapper.hpp>
//! \author Josh Bramlett
//! \version 0.0.2
//! \date 04/13/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <GL/glew.h>
#include <cstddef>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {
namespace OpenGL {

//! \brief Throw exception if OpenGL error flag is set
//! \param [in] func OpenGL function name
//! \throws RDGE::GLException with corresponding error code
inline void
gl_throw_on_error (const char* func)
{
    GLenum code = glGetError();
    if (code != GL_NO_ERROR)
    {
        GL_THROW("OpenGL call failed", func, static_cast<RDGE::UInt32>(code));
    }
}

#ifdef RDGE_DEBUG
#define GL_CHECK_ERROR(x) \
do { \
    x; \
    gl_throw_on_error(#x); \
} while (false)
#else
#define GL_CHECK_ERROR(x) x
#endif

/******************************************************************
 *                          Shaders
 *****************************************************************/

//! \brief Direct map to glCreateShader
//! \details Creates an empty shader object.
//! \param [in] shader_type Type of shader to be created
//! \returns Unique non-zero identifier
//! \see https://www.opengl.org/sdk/docs/man/html/glCreateShader.xhtml
inline RDGE::UInt32
CreateShader (RDGE::UInt32 shader_type)
{
    RDGE::UInt32 handle;
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
SetShaderSource (RDGE::UInt32 shader, const char** source)
{
    GL_CHECK_ERROR(glShaderSource(shader, 1, source, nullptr));
}

//! \brief Direct map to glCompileShader
//! \details Compile a shader object.
//! \param [in] shader Handle of the shader
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glCompileShader.xml
inline void
CompileShader (RDGE::UInt32 shader)
{
    GL_CHECK_ERROR(glCompileShader(shader));
}

//! \brief Direct map to glDeleteShader
//! \details Free up memory allocated to the shader
//! \param [in] shader Handle of the shader
//! \see https://www.opengl.org/sdk/docs/man/html/glDeleteShader.xhtml
inline void
DeleteShader (RDGE::UInt32 shader)
{
    GL_CHECK_ERROR(glDeleteShader(shader));
}

//! \brief Direct map to glCreateProgram
//! \details Create a empty shader program object
//! \returns Unique non-zero identifier
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glCreateProgram.xml
inline RDGE::UInt32
CreateProgram (void)
{
    RDGE::UInt32 handle;
    GL_CHECK_ERROR(handle = glCreateProgram());
    return handle;
}

//! \brief Direct map to glAttachShader
//! \details Attach a shader to a program object.
//! \param [in] program Handle of the program
//! \param [in] shader Handle of the shader
//! \see https://www.opengl.org/sdk/docs/man/html/glAttachShader.xhtml
inline void
AttachShader (RDGE::UInt32 program, RDGE::UInt32 shader)
{
    GL_CHECK_ERROR(glAttachShader(program, shader));
}

//! \brief Direct map to glDetachShader
//! \details Detach a shader from a program object.
//! \param [in] program Handle of the program
//! \param [in] shader Handle of the shader
//! \see https://www.opengl.org/sdk/docs/man/html/glDetachShader.xhtml
inline void
DetachShader (RDGE::UInt32 program, RDGE::UInt32 shader)
{
    GL_CHECK_ERROR(glDetachShader(program, shader));
}

//! \brief Direct map to glLinkProgram
//! \details Links specified program.
//! \param [in] program Handle of the program
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glLinkProgram.xml
inline void
LinkProgram (RDGE::UInt32 program)
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
UseProgram (RDGE::UInt32 program)
{
    GL_CHECK_ERROR(glUseProgram(program));
}

//! \brief Direct map to glGetUniformLocation
//! \details Get the location of a uniform variable
//! \param [in] program Handle of the program
//! \param [in] name Name of the uniform variable
//! \returns Uniform variable location
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetUniformLocation.xml
inline RDGE::Int32
GetUniformLocation (RDGE::UInt32 program, const std::string& name)
{
    RDGE::Int32 result;
    GL_CHECK_ERROR(result = glGetUniformLocation(program, name.c_str()));
    return result;
}

/******************************************************************
 *                       Vertex Arrays
 *****************************************************************/

//! \brief Create a single vertex array from glGenVertexArrays
//! \details Generates a single vertex array object
//! \returns Identifier of the generated vertex array
//! \see https://www.opengl.org/sdk/docs/man/html/glGenVertexArrays.xhtml
inline RDGE::UInt32
CreateVertexArray (void)
{
    RDGE::UInt32 name;
    GL_CHECK_ERROR(glGenVertexArrays(1, &name));
    return name;
}

//! \brief Direct map to glGenVertexArrays
//! \details Generates multiple vertex array object names
//! \param [in] n Number of vertex array names to generate
//! \param [out] arrays Array where generated names are stored
//! \see https://www.opengl.org/sdk/docs/man/html/glGenVertexArrays.xhtml
inline void
CreateVertexArrays (RDGE::Int32 n, RDGE::UInt32* arrays)
{
    GL_CHECK_ERROR(glGenVertexArrays(n, arrays));
}

//! \brief Direct map to glBindVertexArray
//! \details Bind a vertex array object
//! \param [in] name Name of the vertex array to bind
//! \see https://www.opengl.org/sdk/docs/man/html/glBindVertexArray.xhtml
inline void
BindVertexArray (RDGE::UInt32 name)
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

inline void FreeVertexArray(RDGE::UInt32 array)
{
    GL_CHECK_ERROR(glDeleteVertexArrays(1, &array));
}

inline void FreeVertexArrays(RDGE::UInt32 size, RDGE::UInt32* arrays)
{
    GL_CHECK_ERROR(glDeleteVertexArrays(size, arrays));
}

//! \brief Direct map to glEnableVertexAttribArray
//! \details Enables a generic vertex attribute array
//! \param [in] index Index (used by shaders) of the generic vertex attribute
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glEnableVertexAttribArray.xml
inline void
EnableVertexAttribute (RDGE::UInt32 index)
{
    GL_CHECK_ERROR(glEnableVertexAttribArray(index));
}

inline void DisableVertexAttribute(RDGE::UInt32 index)
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
SetVertexAttributePointer (
                           RDGE::UInt32 index,
                           RDGE::Int32  size,
                           RDGE::UInt32 type,
                           bool         normalized,
                           RDGE::UInt32 stride,
                           void*        offset
                          )
{
    GL_CHECK_ERROR(glVertexAttribPointer(
                                         index, size, type,
                                         static_cast<RDGE::UInt32>(normalized),
                                         stride, offset
                                        ));
}

//! \brief Direct map to glDrawElements
//! \details Renders primitives from array data.
//! \param [in] mode Type of primitive to render
//! \param [in] count The number of elements to be rendered
//! \param [in] type The type of values in indices parameter
//! \param [in] indices Pointer to indices storage
//! \see https://www.opengl.org/sdk/docs/man/html/glDrawElements.xhtml
inline void
DrawElements (RDGE::UInt32 mode, RDGE::UInt32 count, RDGE::UInt32 type, const void* indices)
{
    GL_CHECK_ERROR(glDrawElements(mode, count, type, indices));
}

/******************************************************************
 *                           Buffers
 *****************************************************************/

//! \brief Create a single buffer from glGenBuffers
//! \details Generates a single buffer object
//! \returns Identifier of the generated buffer
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGenBuffers.xml
inline RDGE::UInt32
CreateBuffer (void)
{
    RDGE::UInt32 name;
    GL_CHECK_ERROR(glGenBuffers(1, &name));
    return name;
}

//! \brief Direct map to glGenBuffers
//! \details Generates multiple buffer object names
//! \param [in] n Number of buffer names to generate
//! \param [out] buffers Array where generated names are stored
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGenBuffers.xml
inline void
CreateBuffers (RDGE::Int32 n, RDGE::UInt32* buffers)
{
    GL_CHECK_ERROR(glGenBuffers(n, buffers));
}

//! \brief Direct map to glBindBuffer
//! \details Bind the buffer object to the binding target
//! \param [in] target The target to which the buffer is bound
//! \param [in] buffer Identifier of the buffer object
//! \see https://www.khronos.org/opengles/sdk/1.1/docs/man/glBindBuffer.xml
inline void
BindBuffer (RDGE::UInt32 target, RDGE::UInt32 buffer)
{
    GL_CHECK_ERROR(glBindBuffer(target, buffer));
}

//! \brief Break all existing buffer bindings for the target
//! \param [in] target The target to which the buffer will be unbound
//! \see https://www.khronos.org/opengles/sdk/1.1/docs/man/glBindBuffer.xml
inline void
UnbindBuffers (RDGE::UInt32 target)
{
    GL_CHECK_ERROR(glBindBuffer(target, 0));
}

//! \brief Delete the existing buffer with the provided name
//! \param [in] name Name of the buffer
//! \see https://www.opengl.org/sdk/docs/man/html/glDeleteBuffers.xhtml
inline void
DeleteBuffer (RDGE::UInt32 name)
{
    GL_CHECK_ERROR(glDeleteBuffers(1, &name));
}

//! \brief Direct map to glDeleteBuffers
//! \details Deletes n buffer objects named by the elements in the array.
//! \param [in] n Number of buffers to delete
//! \param [in] buffers Array of buffer objects to be deleted
//! \see https://www.opengl.org/sdk/docs/man/html/glDeleteBuffers.xhtml
inline void
DeleteBuffers (RDGE::UInt32 n, RDGE::UInt32* buffers)
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
SetBufferData (RDGE::UInt32 target, std::ptrdiff_t size, const void* data, RDGE::UInt32 usage)
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
GetBufferPointer (RDGE::UInt32 target, RDGE::UInt32 access)
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
ReleaseBufferPointer (RDGE::UInt32 target)
{
    RDGE::UInt32 result;
    GL_CHECK_ERROR(result = glUnmapBuffer(target));
    return result == GL_TRUE;
}

/******************************************************************
 *                         Frame Buffers
 *****************************************************************/

inline RDGE::UInt32 CreateFrameBuffer()
{
    RDGE::UInt32 name;
    GL_CHECK_ERROR(glGenFramebuffers(1, &name));
    return name;
}

inline void CreateFramebuffers(RDGE::UInt32 size, RDGE::UInt32* buffers)
{
    GL_CHECK_ERROR(glGenFramebuffers(size, buffers));
}

inline void BindFramebuffer(RDGE::UInt32 target, RDGE::UInt32 buffer)
{
    GL_CHECK_ERROR(glBindFramebuffer(target, buffer));
}

inline void FreeFramebuffer(RDGE::UInt32 buffer)
{
    GL_CHECK_ERROR(glDeleteFramebuffers(1, &buffer));
}

inline void FreeFramebuffers(RDGE::UInt32 size, RDGE::UInt32* buffers)
{
    GL_CHECK_ERROR(glDeleteFramebuffers(size, buffers));
}

/******************************************************************
 *                         Render Buffers
 *****************************************************************/

inline RDGE::UInt32 CreateRenderBuffer()
{
    RDGE::UInt32 result;
    GL_CHECK_ERROR(glGenRenderbuffers(1, &result));
    return result;
}

inline void CreateRenderbuffers(RDGE::UInt32 size, RDGE::UInt32* buffers)
{
    GL_CHECK_ERROR(glGenRenderbuffers(size, buffers));
}

inline void BindRenderbuffer(RDGE::UInt32 target, RDGE::UInt32 buffer)
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
inline RDGE::UInt32
CreateTexture (void)
{
    RDGE::UInt32 name;
    GL_CHECK_ERROR(glGenTextures(1, &name));
    return name;
}

//! \brief Direct map to glGenTextures
//! \details Generates multiple texture object names
//! \param [in] n Number of texture names to generate
//! \param [out] textures Array where generated names are stored
//! \see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGenTextures.xml
inline void
CreateTextures (RDGE::UInt32 n, RDGE::UInt32* textures)
{
    GL_CHECK_ERROR(glGenTextures(n, textures));
}

//! \brief Direct map to glBindTexture
//! \details Bind the texture object to the binding target
//! \param [in] target The target to which the texture is bound
//! \param [in] texture Identifier of the texture object
//! \see https://www.opengl.org/sdk/docs/man/html/glBindTexture.xhtml
inline void
BindTexture (RDGE::UInt32 target, RDGE::UInt32 texture)
{
    GL_CHECK_ERROR(glBindTexture(target, texture));
}

//! \brief Break all existing texture bindings for the target
//! \param [in] target The target to which the texture will be unbound
//! \see https://www.opengl.org/sdk/docs/man/html/glBindTexture.xhtml
inline void
UnbindTexture (RDGE::UInt32 target)
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
SetTextureParameter (RDGE::UInt32 target, RDGE::UInt32 pname, RDGE::Int32 param)
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
SetTextureData (
                RDGE::UInt32 target,
                RDGE::Int32  internalformat,
                RDGE::Int32  width,
                RDGE::Int32  height,
                RDGE::Int32  format,
                RDGE::UInt32 type,
                const void*  data
               )
{
    GL_CHECK_ERROR(glTexImage2D(
                                target,
                                0,              // level-of-detail
                                internalformat,
                                width,
                                height,
                                0,              // width of the border.  must be 0.
                                format,
                                type,
                                data
                               ));
}

//! \brief Direct map to glActiveTexture
//! \details Set the active texture unit.
//! \param [in] texture Texture unit to activate
//! \see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glActiveTexture.xml
inline void
SetActiveTexture (RDGE::UInt32 texture)
{
    GL_CHECK_ERROR(glActiveTexture(texture));
}

inline void DeleteTexture(RDGE::UInt32 texture)
{
    GL_CHECK_ERROR(glDeleteTextures(1, &texture));
}

inline void DeleteTextures(RDGE::UInt32 size, RDGE::UInt32* textures)
{
    GL_CHECK_ERROR(glDeleteTextures(size, textures));
}

/******************************************************************
 *                      OpenGL Get Values
 *****************************************************************/

//! \brief Query OpenGL for the boolean value of a given parameter
//! \param [in] pname Parameter name
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glGet.xml
inline bool
GetBooleanValue (RDGE::UInt32 pname)
{
    GLboolean value;
    GL_CHECK_ERROR(glGetBooleanv(pname, &value));
    return (value == GL_TRUE);
}

//! \brief Query OpenGL for the float value of a given parameter
//! \param [in] pname Parameter name
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glGet.xml
inline float
GetFloatValue (RDGE::UInt32 pname)
{
    GLfloat value;
    GL_CHECK_ERROR(glGetFloatv(pname, &value));
    return value;
}

//! \brief Query OpenGL for the integer value of a given parameter
//! \param [in] pname Parameter name
//! \see https://www.opengl.org/sdk/docs/man2/xhtml/glGet.xml
inline RDGE::Int32
GetIntegerValue (RDGE::UInt32 pname)
{
    GLint value;
    GL_CHECK_ERROR(glGetIntegerv(pname, &value));
    return static_cast<RDGE::Int32>(value);
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

inline void SetBufferSubData(RDGE::UInt32 target, ptrdiff_t offset, ptrdiff_t size, const void* data)
{
    GL_CHECK_ERROR(glBufferSubData(target, offset, size, data));
}

inline void FramebufferTexture2D(RDGE::UInt32 target, RDGE::UInt32 attachment, RDGE::UInt32 textarget, RDGE::UInt32 texture, int level)
{
    GL_CHECK_ERROR(glFramebufferTexture2D(target, attachment, textarget, texture, level));
}

inline void FramebufferRenderbuffer(RDGE::UInt32 target, RDGE::UInt32 attachment, RDGE::UInt32 renderbuffertarget, RDGE::UInt32 renderbuffer)
{
    GL_CHECK_ERROR(glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer));
}

inline void RenderbufferStorage(RDGE::UInt32 target, RDGE::UInt32 format, RDGE::UInt32 width, RDGE::UInt32 height)
{
    GL_CHECK_ERROR(glRenderbufferStorage(target, format, width, height));
}

inline void SetViewport(RDGE::UInt32 x, RDGE::UInt32 y, RDGE::UInt32 width, RDGE::UInt32 height)
{
    GL_CHECK_ERROR(glViewport(x, y, width, height));
}

inline void Clear(RDGE::UInt32 flags)
{
    GL_CHECK_ERROR(glClear(flags));
}

inline void SetClearColor(float r, float g, float b, float a)
{
    GL_CHECK_ERROR(glClearColor(r, g, b, a));
}


} // namespace OpenGL
} // namespace Graphics
} // namespace RDGE
