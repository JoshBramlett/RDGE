//! \headerfile <rdge/util/containers/enum_array.hpp>
//! \author Josh Bramlett
//! \version 0.0.11
//! \date 11/02/2018

#pragma once

#include <rdge/core.hpp>
#include <rdge/type_traits.hpp>
#include <rdge/math/intrinsics.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \brief Array of type T indexable by the enum E
//! \warning Do not use if the enum values don't follow a standard increment.
template <typename T, typename E, size_t Size>
class EnumArray
{
public:
    T& operator[] (E e) { return m_arr[to_underlying(e)]; }
    const T& operator[] (E e) const { return m_arr[to_underlying(e)]; }

private:
    T m_arr[Size];
};

//! \brief Array of type T indexable by the bitmask E
//! \note Element preferance is given to the least significant bit
template <typename T, typename E, size_t Size>
class EnumBitmaskArray
{
public:
    T& operator[] (E e) { return m_arr[math::lsb(to_underlying(e)) - 1]; }
    const T& operator[] (E e) const { return m_arr[math::lsb(to_underlying(e)) - 1]; }

private:
    T m_arr[Size];
};

} // namespace rdge
