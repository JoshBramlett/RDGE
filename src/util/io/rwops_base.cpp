#include <rdge/util/io/rwops_base.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/debug/assert.hpp>

#include <sstream>

namespace rdge {

std::string
GetTextFileContent (const char* filepath)
{
    auto rwops = rwops_base::from_file(filepath, "rt");
    auto sz = rwops.size();

    std::string result;
    if (RDGE_UNLIKELY((sz + 1u) > result.max_size()))
    {
        std::ostringstream ss;
        ss << "File contents exceed max size."
           << " filepath=" << filepath
           << " size=" << (sz + 1u)
           << " max_size=" << result.max_size();
        RDGE_THROW(ss.str());
    }

    result = std::string((sz + 1u), '\0');
    rwops.read(result.data(), sizeof(char), sz);

    return result;
}

rwops_base::rwops_base (SDL_RWops* sdl_rwops)
    : m_rwops(sdl_rwops)
{ }

rwops_base::~rwops_base (void) noexcept
{
    if (m_rwops)
    {
        SDL_RWclose(m_rwops);
    }
}

rwops_base::rwops_base (rwops_base&& rhs) noexcept
    : m_rwops(rhs.m_rwops)
{
    rhs.m_rwops = nullptr;
}

rwops_base&
rwops_base::operator= (rwops_base&& rhs) noexcept
{
    if (this != &rhs)
    {
        std::swap(m_rwops, rhs.m_rwops);
    }

    return *this;
}

uint64
rwops_base::size (void)
{
    int64 sz = SDL_RWsize(m_rwops);
    if (RDGE_UNLIKELY(sz < 0))
    {
        SDL_THROW("Failed to get file size", "SDL_RWsize");
    }

    return static_cast<uint64>(sz);
}

int64
rwops_base::seek (int64 offset, seekdir whence)
{
    return SDL_RWseek(m_rwops, offset, to_underlying(whence));
}

size_t
rwops_base::read (void* ptr, size_t size, size_t count)
{
    size_t result = SDL_RWread(m_rwops, ptr, size, count);
    if (RDGE_UNLIKELY(result == 0))
    {
        SDL_THROW("Failed to read from file", "SDL_RWread");
    }

    return result;
}

size_t
rwops_base::write (void* ptr, size_t size, size_t count)
{
    size_t result = SDL_RWwrite(m_rwops, ptr, size, count);
    if (RDGE_UNLIKELY(result != count))
    {
        SDL_THROW("Failed to write to file", "SDL_RWread");
    }

    return result;
}

int32
rwops_base::close (void)
{
    int32 result = SDL_RWclose(m_rwops);
    m_rwops = nullptr;

    return result;
}

int64
rwops_base::tell (void) const noexcept
{
    return SDL_RWtell(m_rwops);
}

/* static */ rwops_base
rwops_base::from_file (const char* file, const char* mode)
{
    RDGE_ASSERT(file != nullptr);
    RDGE_ASSERT(mode != nullptr);

    SDL_RWops* sdl_rwops = SDL_RWFromFile(file, mode);
    if (RDGE_UNLIKELY(!sdl_rwops))
    {
        SDL_THROW("Failed to create rwops from file", "SDL_RWFromFile");
    }

    return rwops_base(sdl_rwops);
}

/* static */ rwops_base
rwops_base::from_fp (FILE* file, bool autoclose)
{
    RDGE_ASSERT(file != nullptr);

    SDL_RWops* sdl_rwops = SDL_RWFromFP(file, autoclose ? SDL_TRUE : SDL_FALSE);
    if (RDGE_UNLIKELY(!sdl_rwops))
    {
        SDL_THROW("Failed to create rwops from file pointer", "SDL_RWFromFP");
    }

    return rwops_base(sdl_rwops);
}

/* static */ rwops_base
rwops_base::from_memory (void* mem, int32 size)
{
    RDGE_ASSERT(mem != nullptr);
    RDGE_ASSERT(size > 0);

    SDL_RWops* sdl_rwops = SDL_RWFromMem(mem, size);
    if (RDGE_UNLIKELY(!sdl_rwops))
    {
        SDL_THROW("Failed to create rwops from memory buffer", "SDL_RWFromMem");
    }

    return rwops_base(sdl_rwops);
}

/* static */ rwops_base
rwops_base::from_const_memory (const void* mem, int32 size)
{
    RDGE_ASSERT(mem != nullptr);
    RDGE_ASSERT(size > 0);

    SDL_RWops* sdl_rwops = SDL_RWFromConstMem(mem, size);
    if (RDGE_UNLIKELY(!sdl_rwops))
    {
        SDL_THROW("Failed to create rwops from const memory buffer", "SDL_RWFromConstMem");
    }

    return rwops_base(sdl_rwops);
}

} // namespace rdge
