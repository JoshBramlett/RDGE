#pragma once

#include <rdge/types.hpp>

#include <GL/glew.h>
#include <cstddef>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {
namespace OpenGL {

//! \brief Throw exception if OpenGL error flag is set
//! \throws RDGE::GLException with corresponding error code
void gl_throw_on_error (const char* func);

#ifdef RDGE_THROW_ON_GL_ERROR
#define GL_CHECK_ERROR(x) \
do { \
    x; \
    gl_throw_on_error(#x); \
} while (false)
#else
#define GL_CHECK_ERROR(x) x
#endif

/*
 * Shader
 */

//! \brief Direct map to glCreateShader
//! \details Creates an empty shader object.
//! \param [in] shader_type Type of shader to be created
//! \returns Unique non-zero identifier
//! \see
inline RDGE::UInt32
CreateShader (RDGE::UInt32 shader_type)
{
    RDGE::UInt32 handle;
    GL_CHECK_ERROR(handle = glCreateShader(shader_type));
    return handle;
}

//! \brief Direct map to glShaderSource
//! \details Replaces source code in a shader object.  Wrapper assumes source is
//!          a null terminated string.
//! \param [in] shader Handle of the shader
//! \param [in] source Source code to be loaded into the shader
//! \see
inline void
SetShaderSource (RDGE::UInt32 shader, const char** source)
{
    GL_CHECK_ERROR(glShaderSource(shader, 1, source, nullptr));
}

//! \brief Direct map to glCompileShader
//! \details Compile a shader object.
//! \param [in] shader Handle of the shader
//! \see
inline void
CompileShader (RDGE::UInt32 shader)
{
    GL_CHECK_ERROR(glCompileShader(shader));
}

//! \brief Direct map to glDeleteShader
//! \details Free up memory allocated to the shader
//! \param [in] shader Handle of the shader
//! \see
inline void
DeleteShader (RDGE::UInt32 shader)
{
    GL_CHECK_ERROR(glDeleteShader(shader));
}

//! \brief Direct map to glCreateProgram
//! \details Create a empty shader program object
//! \returns Unique non-zero identifier
//! \see
inline RDGE::UInt32
CreateProgram (void)
{
    RDGE::UInt32 handle;
    GL_CHECK_ERROR(handle = glCreateProgram());
    return handle;
}

//! \brief Direct map to glAttachShader
//! \details Attach a shader to a program object
//! \param [in] program Handle of the program
//! \param [in] shader Handle of the shader
//! \see
inline void
AttachShader (RDGE::UInt32 program, RDGE::UInt32 shader)
{
    GL_CHECK_ERROR(glAttachShader(program, shader));
}

//! \brief Direct map to glDetachShader
//! \details Detach a shader from a program object
//! \param [in] program Handle of the program
//! \param [in] shader Handle of the shader
//! \see
inline void
DetachShader (RDGE::UInt32 program, RDGE::UInt32 shader)
{
    GL_CHECK_ERROR(glDetachShader(program, shader));
}

//! \brief Direct map to glLinkProgram
//! \details Links specified program
//! \param [in] program Handle of the program
//! \see
inline void
LinkProgram (RDGE::UInt32 program)
{
    GL_CHECK_ERROR(glLinkProgram(program));
}

/*
 * Vertex Array
 */

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

inline void FreeArray(RDGE::UInt32 array)
{
    GL_CHECK_ERROR(glDeleteVertexArrays(1, &array));
}

inline void FreeVertexArray(RDGE::UInt32 array)
{
    GL_CHECK_ERROR(glDeleteVertexArrays(1, &array));
}

inline void FreeVertexArrays(RDGE::UInt32 size, RDGE::UInt32* arrays)
{
    GL_CHECK_ERROR(glDeleteVertexArrays(size, arrays));
}

/*
 * Buffers
 */

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

/*
 * Frame Buffers
 */

inline RDGE::UInt32 CreateFrameBuffer()
{
    RDGE::UInt32 result;
    GL_CHECK_ERROR(glGenFramebuffers(1, &result));
    return result;
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

/*
 * Render Buffers
 */

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

inline RDGE::UInt32 CreateTexture()
{
    GLuint result;
    GL_CHECK_ERROR(glGenTextures(1, &result));
    return result;
}

inline void CreateTextures(RDGE::UInt32 size, RDGE::UInt32* textures)
{
    GL_CHECK_ERROR(glGenTextures(size, textures));
}

inline void BindTexture(RDGE::UInt32 target, RDGE::UInt32 texture)
{
    GL_CHECK_ERROR(glBindTexture(target, texture));
}

inline void UnbindTexture(RDGE::UInt32 target)
{
    GL_CHECK_ERROR(glBindTexture(target, 0));
}

inline void SetTextureParameter(RDGE::UInt32 target, RDGE::UInt32 parameter, int value)
{
    GL_CHECK_ERROR(glTexParameteri(target, parameter, value));
}

inline void SetTextureData(RDGE::UInt32 target, RDGE::UInt32 internalformat, RDGE::UInt32 width, RDGE::UInt32 height, RDGE::UInt32 format, RDGE::UInt32 type, const void* pixels)
{
    GL_CHECK_ERROR(glTexImage2D(target, 0, internalformat, width, height, 0, format, type, pixels));
}

inline void SetActiveTexture(RDGE::UInt32 texture)
{
    GL_CHECK_ERROR(glActiveTexture(texture));
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

inline void FreeTexture(RDGE::UInt32 texture)
{
    GL_CHECK_ERROR(glDeleteTextures(1, &texture));
}

inline void FreeTextures(RDGE::UInt32 size, RDGE::UInt32* textures)
{
    GL_CHECK_ERROR(glDeleteTextures(size, textures));
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

} // namespace OpenGL
} // namespace Graphics
} // namespace RDGE
