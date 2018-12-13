//! \headerfile <rdge/util/io/rwops_base.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 07/31/2017

#pragma once

#include <rdge/core.hpp>

#include <SDL_rwops.h>

#include <cstdio>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \brief Read and return contents of a text file
//! \param [in] filepath Path of the file to read
//! \throws rdge::Exception File too large to be read
//! \throws rdge::SDLException Inner RWOPS exception
std::string
GetTextFileContent (const char* filepath);

//! \class rwops_base
//! \brief Wrapper for SDL_RWops, an abstract interface to stream i/o
//! \details Base class inherits the functionality provided by SDL which
//!          includes those provided through static initializers.  Derived
//!          classes must allocate their own native handle and assign
//!          all corresponding interface callbacks.  Freeing the native
//!          handle must be done in the close() function, not the
//!          destructor.  For an example implementation, see the
//!          SDL_AllocRW documentation.
//! \see https://wiki.libsdl.org/SDL_AllocRW
class rwops_base
{
public:
    //! \enum seekdir
    //! \brief File seeking direction type
    enum class seekdir
    {
        beg = RW_SEEK_SET, //!< Beginning of a stream
        cur = RW_SEEK_CUR, //!< Current stream cursor position
        end = RW_SEEK_END  //!< End of a stream
    };

    //! \brief rwops_base ctor
    //! \details Take ownership of an existing SDL_RWops.
    //! \param [in] sdl_rwops Native SDL_RWops
    explicit rwops_base (SDL_RWops* sdl_rwops);

    //! \brief rwops_base ctor
    virtual ~rwops_base (void) noexcept;

    //!@{ Non-copyable, move enabled
    rwops_base (const rwops_base&) = delete;
    rwops_base& operator= (const rwops_base&) = delete;
    rwops_base (rwops_base&&) noexcept;
    rwops_base& operator= (rwops_base&&) noexcept;
    //!@}

    //!@{
    //! \brief User defined conversion to a raw SDL_RWops pointer
    //! \warning Pointer will be invalidated when parent object is destroyed
    explicit operator const SDL_RWops* (void) const noexcept { return m_rwops; }
    explicit operator SDL_RWops* (void) const noexcept { return m_rwops; }
    //!@}

    //!@{
    //! \brief Stream interface
    //! \details Each method calls the corresponding callback defined in the
    //!          native SDL_RWops object.  Derived classes must set those
    //!          function pointers for proper behavior.
    //! \note All functions are based on their corresponding stdio functions
    //! \throws rdge::SDLException Failed to read or write
    size_t size (void);
    int64 seek (int64 offset, seekdir whence);
    int64 tell (void) const noexcept;
    size_t read (void* ptr, size_t size, size_t count = 1);
    size_t write (void* ptr, size_t size, size_t count = 1);
    int32 close (void);
    //!@}

    //!@{
    //! \brief Static constructors
    //! \throws rdge::SDLException Failed to construct object
    static rwops_base from_file (const char* file, const char* mode);
    static rwops_base from_fp (FILE* file, bool autoclose);
    static rwops_base from_memory (void* mem, int32 size);
    static rwops_base from_const_memory (const void* mem, int32 size);
    //!@}

protected:
    SDL_RWops* m_rwops = nullptr;
};

} // namespace rdge
