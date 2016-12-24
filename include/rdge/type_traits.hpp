//! \headerfile <rdge/type_traits.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

#pragma once

#include <type_traits>

namespace rdge {

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
//! \warning Explicit template must be declared in the rdge namespace
template <typename T, typename = typename std::enable_if_t<std::is_enum<T>::value>>
struct is_enum_bitmask : public std::false_type { };

// Alternate version that does not require the type to be an enum
//template <typename>
//struct is_enum_bitmask : public std::false_type { };

} // namespace rdge

//! \brief enum_bitmask_t bitwise OR operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Bitwise OR result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T>
operator| (T lhs, T rhs)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) |
                          static_cast<std::underlying_type_t<T>>(rhs));
}

//! \brief enum_bitmask_t bitwise AND operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Bitwise AND result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T>
operator& (T lhs, T rhs)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) &
                          static_cast<std::underlying_type_t<T>>(rhs));
}

//! \brief enum_bitmask_t bitwise XOR operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Bitwise XOR result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T>
operator^ (T lhs, T rhs)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) ^
                          static_cast<std::underlying_type_t<T>>(rhs));
}

//! \brief enum_bitmask_t bitwise OR assignment operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Left side reference assigned bitwise OR result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T&>
operator|= (T& lhs, T rhs)
{
    lhs = static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) |
                         static_cast<std::underlying_type_t<T>>(rhs));
    return lhs;
}

//! \brief enum_bitmask_t bitwise AND assignment operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Left side reference assigned bitwise AND result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T&>
operator&= (T& lhs, T rhs)
{
    lhs = static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) &
                         static_cast<std::underlying_type_t<T>>(rhs));
    return lhs;
}

//! \brief enum_bitmask_t bitwise XOR assignment operator
//! \param [in] lhs Left side value
//! \param [in] lhs Right side value
//! \returns Left side reference assigned bitwise XOR result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T&>
operator^= (T& lhs, T rhs)
{
    lhs = static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) ^
                         static_cast<std::underlying_type_t<T>>(rhs));
    return lhs;
}

//! \brief enum_bitmask_t unary bitwise NOT operator
//! \param [in] value enum_bitmask_t value
//! \returns Self reference assigned bitwise NOT result
template <typename T>
constexpr typename std::enable_if_t<rdge::is_enum_bitmask<T>::value, T&>
operator~ (T& value)
{
    value = static_cast<T>(~static_cast<std::underlying_type_t<T>>(value));
    return value;
}
