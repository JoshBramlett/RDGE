//! \headerfile <rdge/type_traits.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

#pragma once

#include <rdge/core.hpp>

#include <type_traits>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \brief Cast enum to it's underlying type
//! \details Used for code clarity and brevity
//! \param [in] value Enumeration value
//! \returns Underlying type value
template <typename T, typename = typename std::enable_if_t<std::is_enum<T>::value>>
constexpr auto to_underlying (T value) noexcept -> std::underlying_type_t<T>
{
    return static_cast<std::underlying_type_t<T>>(value);
}

//! \brief Sanity check for unexpected enum additions
//! \note For use in static asserts
//! \param [in] last The last expected enum
//! \param [in] count the "count" enum
//! \returns Correctness of expected enums
template <typename T, typename = typename std::enable_if_t<std::is_enum<T>::value>>
constexpr bool enum_sanity_check (T last, T count)
{
    return ((to_underlying(count) - to_underlying(last)) == 1);
}

//! \struct is_enum_bitmask
//! \brief Type traits check that an enum is a bitmask
//! \details Bitwise operator overloads are provided for enabled types.  Enums must
//!          explicitly enable themselves for support.
//! \code{.cpp}
//! enum class MyEnum : rdge::uint8
//! {
//!     RED   = 0x01,
//!     GREEN = 0x02,
//!     BLUE  = 0x04
//! };
//!
//! // Enable MyEnum for bitmask operations
//! template<>
//! struct is_enum_bitmask<MyEnum> : public std::true_type { };
//! \endcode
//! \warning Underlying type must be unsigned
template <typename T, typename = typename std::enable_if_t<std::is_enum<T>::value>>
struct is_enum_bitmask : std::false_type { };

} // namespace rdge


//! \defgroup Enum bitmask operations
//!@{

//! \brief enum_bitmask_t equality operator
//! \details Used for set bit checking operations.  e.g.
//! \code{.cpp}
//! if ((my_enum_bitmask & MyEnum::RED) == 0) { ... }
//! \endcode
//! \param [in] lhs Enum value to compare
//! \param [in] scalar Scalar to compare
//! \returns True iff underlying value matches the scalar
template <typename T, typename U>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value &&
                                    std::is_integral<U>::value, bool>
operator== (T lhs, U scalar) noexcept
{
    static_assert(std::is_unsigned<std::underlying_type_t<T>>::value,
                  "is_enum_bitmask requires an unsigned underlying type");

    // casted to scalar type for proper bool behavior
    return (static_cast<U>(lhs) == scalar);
}

//! \brief enum_bitmask_t inequality operator
//! \param [in] lhs Enum value to compare
//! \param [in] scalar Scalar to compare
//! \returns True iff underlying value does not match the scalar
template <typename T, typename U>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value &&
                                    std::is_integral<U>::value, bool>
operator!= (T lhs, U scalar) noexcept
{
    return !(lhs == scalar);
}

//! \brief enum_bitmask_t unary bitwise NOT operator
//! \param [in] value enum_bitmask_t value
//! \returns Bitwise NOT result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T>
operator~ (T value) noexcept
{
    static_assert(std::is_unsigned<std::underlying_type_t<T>>::value,
                  "is_enum_bitmask requires an unsigned underlying type");

    return static_cast<T>(~rdge::to_underlying(value));
}

//! \brief enum_bitmask_t bitwise OR operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Bitwise OR result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T>
operator| (T lhs, T rhs) noexcept
{
    static_assert(std::is_unsigned<std::underlying_type_t<T>>::value,
                  "is_enum_bitmask requires an unsigned underlying type");

    return static_cast<T>(rdge::to_underlying(lhs) | rdge::to_underlying(rhs));
}

//! \brief enum_bitmask_t bitwise AND operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Bitwise AND result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T>
operator& (T lhs, T rhs) noexcept
{
    static_assert(std::is_unsigned<std::underlying_type_t<T>>::value,
                  "is_enum_bitmask requires an unsigned underlying type");

    return static_cast<T>(rdge::to_underlying(lhs) & rdge::to_underlying(rhs));
}

//! \brief enum_bitmask_t bitwise XOR operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Bitwise XOR result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T>
operator^ (T lhs, T rhs) noexcept
{
    static_assert(std::is_unsigned<std::underlying_type_t<T>>::value,
                  "is_enum_bitmask requires an unsigned underlying type");

    return static_cast<T>(rdge::to_underlying(lhs) ^ rdge::to_underlying(rhs));
}

//! \brief enum_bitmask_t bitwise OR assignment operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Left side reference assigned bitwise OR result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T&>
operator|= (T& lhs, T rhs) noexcept
{
    lhs = lhs | rhs;
    return lhs;
}

//! \brief enum_bitmask_t bitwise AND assignment operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Left side reference assigned bitwise AND result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T&>
operator&= (T& lhs, T rhs) noexcept
{
    lhs = lhs & rhs;
    return lhs;
}

//! \brief enum_bitmask_t bitwise XOR assignment operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Left side reference assigned bitwise XOR result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T&>
operator^= (T& lhs, T rhs) noexcept
{
    lhs = lhs ^ rhs;
    return lhs;
}

//!@}
