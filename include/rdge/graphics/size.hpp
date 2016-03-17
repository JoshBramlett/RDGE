//! \headerfile <rdge/graphics/size.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/22/2015
//! \bug

#pragma once

#include <ostream>

#include <rdge/types.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class Size
//! \brief Structure of width and height values
class Size final
{
public:
    //! \var RDGE::UInt32 w Width
    RDGE::UInt32 w;
    //! \var RDGE::UInt32 h Height
    RDGE::UInt32 h;

public:
    //! \brief Size ctor
    //! \details Initialize size to [0,0]
    constexpr Size ()
        : w(0)
        , h(0)
    { }

    //! \brief Size ctor
    //! \details Initialize size from X and Y values
    //! \param [in] width Width
    //! \param [in] height Height
    constexpr Size (RDGE::UInt32 width, RDGE::UInt32 height)
        : w(width)
        , h(height)
    { }

    //! \brief Size Copy ctor
    //! \details Default-copyable
    Size (const Size&) noexcept = default;

    //! \brief Size Move ctor
    //! \details Default-movable
    Size (Size&&) noexcept = default;

    //! \brief Size Copy Assignment Operator
    //! \details Default-copyable
    Size& operator=(const Size&) noexcept = default;

    //! \brief Size Move Assignment Operator
    //! \details Default-movable
    Size& operator=(Size&&) noexcept = default;

    //! \brief Size memberwise negation deleted
    constexpr Size operator-() = delete;

    //! \brief Size memberwise addition
    //! \param [in] rhs Size to add
    //! \returns Copy of lvalue after rvalue memberwise addition
    constexpr Size operator+(const Size& rhs) const
    {
        return Size(w + rhs.w, h + rhs.h);
    }

    //! \brief Size memberwise subtraction
    //! \param [in] rhs Size to subtract
    //! \returns Copy of lvalue after rvalue memberwise subtraction
    constexpr Size operator-(const Size& rhs) const
    {
        return Size(w - rhs.w, h - rhs.h);
    }

    //! \brief Size memberwise multiplication
    //! \param [in] rhs Size to multiply
    //! \returns Copy of lvalue after rvalue memberwise multiplication
    constexpr Size operator*(const Size& rhs) const
    {
        return Size(w * rhs.w, h * rhs.h);
    }

    //! \brief Size memberwise multiplication
    //! \param [in] value Multiplier of both x and y values
    //! \returns Copy of lvalue after rvalue memberwise multiplication
    constexpr Size operator*(RDGE::UInt32 value) const
    {
        return Size(w * value, h * value);
    }

    //! \brief Size memberwise division
    //! \param [in] rhs Size representing the divisor
    //! \returns Copy of dividend lvalue after rvalue memberwise division
    constexpr Size operator/(const Size& rhs) const
    {
        return Size(w / rhs.w, h / rhs.h);
    }

    //! \brief Size memberwise division
    //! \param [in] value Divisor of both x and y values
    //! \returns Copy of dividend lvalue after rvalue memberwise division
    constexpr Size operator/(RDGE::UInt32 value) const
    {
        return Size(w / value, h / value);
    }

    //! \brief Size memberwise modulo (remainder) operation
    //! \param [in] rhs Size representing the divisor
    //! \returns Copy of dividend lvalue containing the modulus after
    //!          rvalue memberwise division
    constexpr Size operator%(const Size& rhs) const
    {
        return Size(w % rhs.w, h % rhs.h);
    }

    //! \brief Size memberwise modulo (remainder) operation
    //! \param [in] value Divisor of both x and y values
    //! \returns Copy of dividend lvalue containing the modulus after
    //!          rvalue memberwise division
    constexpr Size operator%(RDGE::UInt32 value) const
    {
        return Size(w % value, h % value);
    }

    //! \brief Size memberwise addition
    //! \param [in] rhs Size to add
    //! \returns Reference to self
    Size& operator+=(const Size& rhs)
    {
        w += rhs.w;
        h += rhs.h;
        return *this;
    }

    //! \brief Size memberwise subtraction
    //! \param [in] rhs Size to subtract
    //! \returns Reference to self
    Size& operator-=(const Size& rhs)
    {
        w -= rhs.w;
        h -= rhs.h;
        return *this;
    }

    //! \brief Size memberwise multiplication
    //! \param [in] rhs Size to multiply
    //! \returns Reference to self
    Size& operator*=(const Size& rhs)
    {
        w *= rhs.w;
        h *= rhs.h;
        return *this;
    }

    //! \brief Size memberwise multiplication
    //! \param [in] value Multiplier of both x and y values
    //! \returns Reference to self
    Size& operator*=(RDGE::UInt32 value)
    {
        w *= value;
        h *= value;
        return *this;
    }

    //! \brief Size memberwise division
    //! \param [in] rhs Size representing the divisor
    //! \returns Reference to self
    Size& operator/=(const Size& rhs)
    {
        w /= rhs.w;
        h /= rhs.h;
        return *this;
    }

    //! \brief Size memberwise division
    //! \param [in] value Divisor of both x and y values
    //! \returns Reference to self (dividend)
    Size& operator/=(RDGE::UInt32 value)
    {
        w /= value;
        h /= value;
        return *this;
    }

    //! \brief Size memberwise modulo (remainder) operation
    //! \param [in] rhs Size representing the divisor
    //! \returns Reference to self (dividend)
    Size& operator%=(const Size& rhs)
    {
        w %= rhs.w;
        h %= rhs.h;
        return *this;
    }

    //! \brief Size memberwise modulo (remainder) operation
    //! \param [in] value Divisor of both x and y values
    //! \returns Reference to self (dividend)
    Size& operator%=(RDGE::UInt32 value)
    {
        w %= value;
        h %= value;
        return *this;
    }

    //! \brief Check if structure is empty
    //! \returns True if w and h are zero, otherwise false
    bool IsEmpty (void) const
    {
        return (w == 0) && (h == 0);
    }
};

//! \brief Size equality operator
//! \param [in] a First size to compare
//! \param [in] b Second size to compare
//! \returns True iff sizes are identical
constexpr bool operator==(const Size& a, const Size& b)
{
    return (a.w == b.w) && (a.h == b.h);
}

//! \brief Size inequality operator
//! \param [in] a First size to compare
//! \param [in] b Second size to compare
//! \returns True iff sizes are not identical
constexpr bool operator!=(const Size& a, const Size& b)
{
    return (a.w != b.w) || (a.h != b.h);
}

//! \brief Size stream output operator
//! \param [in] os Output stream
//! \param [in] point Size to write to the stream
//! \returns Output stream
inline std::ostream& operator<< (std::ostream& os, const Size& size)
{
    return os << "[" << size.w << "," << size.h << "]";
}

} // namespace Graphics
} // namespace RDGE
