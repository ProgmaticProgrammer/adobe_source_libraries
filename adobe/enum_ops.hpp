/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ENUM_OPS_HPP
#define ADOBE_ENUM_OPS_HPP

/*************************************************************************************************/

#include <type_traits>

/*************************************************************************************************/

/*!
    \defgroup enum_ops Typesafe Integers and Bit Fields (enums)
    \ingroup utility

    \section Description Description

    \c enum_ops provides optional typesafe bitset and arithmetic operations for enumeration types.
    Without these typesafe operations, the compiler will promote the operand(s) to the appropriate
    integral type, and the result will be an integral type. When the typesafe operations have been
    defined for an enumeration type, \c E, the result will be of type \c E exactly when the
    operand(s) are of type \c E.

    \c ADOBE_DEFINE_BITSET_OPS(E) or auto stlab_enable_bitmask_enum(E) -> std::true_type;
    enables the bitset operations <code>~, |, &, ^, |=, &=, ^= </code>
    for enumeration type \c E.

    \c ADOBE_DEFINE_ARITHMETIC_OPS(E) or auto stlab_enable_arithmetic_enum(E) -> std::true_type;
    enables the typesafe arithmetic operations <code>+, -, *, /,
    %, +=, *=, -=, /=, \%=</code> for enumeration type \c E.

    \section Definition Definition

    Defined in \link enum_ops.hpp <code>adobe/enum_ops.hpp</code> \endlink

    \section Example Example

    The following is an example of code that will compile:
    \dontinclude enum_ops_example.cpp
    \skip start_of_example
    \until end_of_example

    The following is contains an example of code that will not compile
    since the typesafe operators have not been defined.

    \dontinclude enum_ops_example_fail.cpp
    \skip start_of_example
    \until end_of_example
*/

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

auto stlab_enable_bitmask_enum(...) -> std::false_type;
auto stlab_enable_arithmetic_enum(...) -> std::false_type;

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

#if !defined(ADOBE_NO_DOCUMENTATION)

template <class T>
constexpr bool has_enabled_bitmask = decltype(stlab_enable_bitmask_enum(std::declval<T>()))::value;
template <class T>
constexpr bool has_enabled_arithmetic =
    decltype(stlab_enable_arithmetic_enum(std::declval<T>()))::value;

template <class, bool>
struct safe_underlying_type;

template <class T>
struct safe_underlying_type<T, true> {
    using type = std::underlying_type_t<T>;
};

template <class T>
struct safe_underlying_type<T, false> {
    using type = void;
};

template <class T>
using safe_underlying_type_t = typename safe_underlying_type<T, std::is_enum<T>::value>::type;

template <class U, class T>
using is_convertible_to_underlying =
    std::is_convertible<U, adobe::implementation::safe_underlying_type_t<T>>;

#endif

/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

// this exist to mantain backwards compatability with the old ops
#define ADOBE_DEFINE_BITSET_OPS(EnumType) auto stlab_enable_bitmask_enum(EnumType)->std::true_type;

template <class T>
constexpr auto operator&(const T lhs,const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_bitmask<T>, T>
{
    using underlying =  std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template <class T>
constexpr auto operator~(const T a) ->
    std::enable_if_t<adobe::implementation::has_enabled_bitmask<T>, T>
{
	using underlying = std::underlying_type_t<T>;
    return static_cast<T>(
        ~static_cast<underlying>(a));
}

template <class T>
constexpr auto operator|(const T lhs, const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_bitmask<T>, T>
{
    using underlying =  std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template <class T>
constexpr auto operator^(const T lhs, const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_bitmask<T>, T>
{
    using underlying =  std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template <class T>
constexpr auto operator^=(T& lhs, const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_bitmask<T>, T&>
{
	return lhs = lhs ^ rhs;
}

template <class T>
constexpr auto operator&=(T& lhs, const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_bitmask<T>, T&>
{
	return lhs = lhs & rhs;
}

template <class T>
constexpr auto operator|=(T& lhs, const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_bitmask<T>, T&>
{
	return lhs = lhs | rhs;
}

// this exist to mantain backwards compatability with the old ops
#define ADOBE_DEFINE_ARITHMETIC_OPS(EnumType) \
    auto stlab_enable_arithmetic_enum(EnumType)->std::true_type;

template <class T>
constexpr auto operator+(const T a) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T>
{
	using underlying = std::underlying_type_t<T>;
    return static_cast<T>(
        +static_cast<underlying>(a));
}

template <class T>
constexpr auto operator-(const T a) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T>
{
	using underlying = std::underlying_type_t<T>;
    return static_cast<T>(
        -static_cast<underlying>(a));
}

template <class T>
constexpr auto operator+(const T lhs,const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T>
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) + static_cast<underlying>(rhs));
}

template <class T>
constexpr auto operator-(const T lhs,const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T>
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) - static_cast<underlying>(rhs));
}

template <class T, class U>
constexpr auto operator*(const T lhs, const U rhs) -> std::enable_if_t<
    adobe::implementation::has_enabled_arithmetic<T> &&
        adobe::implementation::is_convertible_to_underlying<U, T>::value,
    T> {
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) * rhs);
}

template <class U, class T>
constexpr auto operator*(const U lhs, const T rhs) -> std::enable_if_t<
    adobe::implementation::has_enabled_arithmetic<T> &&
        adobe::implementation::is_convertible_to_underlying<U, T>::value,
    T> {
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(lhs * static_cast<underlying>(rhs));
}

template <class T, class U>
constexpr auto operator/(const T lhs, const U rhs)
    -> std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T> &&
                            adobe::implementation::is_convertible_to_underlying<U, T>::value,
                        T> {
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) / rhs);
}

template <class T, class U>
constexpr auto operator%(const T lhs, const U rhs)
    -> std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T> &&
                            adobe::implementation::is_convertible_to_underlying<U, T>::value,
                        T> {
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) % rhs);
}

template <class T>
constexpr auto operator+=(T& lhs, const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T&>
{
	return lhs = lhs + rhs;
}

template <class T>
constexpr auto operator-=(T& lhs, const T rhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T&>
{
	return lhs = lhs - rhs;
}

template <class T, class U>
constexpr auto operator*=(T& lhs, const U rhs)
    -> std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T> &&
                            adobe::implementation::is_convertible_to_underlying<U, T>::value,
                        T&> {
    return lhs = lhs * rhs;
}

template <class T, class U>
constexpr auto operator/=(T& lhs, const U rhs)
    -> std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T> &&
                            adobe::implementation::is_convertible_to_underlying<U, T>::value,
                        T&> {
    return lhs = lhs / rhs;
}

template <class T, class U>
constexpr auto operator%=(T& lhs, const U rhs)
    -> std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T> &&
                            adobe::implementation::is_convertible_to_underlying<U, T>::value,
                        T&> {
    return lhs = lhs % rhs;
}

template <class T>
constexpr auto operator++(T& lhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T&>
{
	return lhs += static_cast<T>(1);
}

template <class T>
constexpr auto operator++(T& lhs, int) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T>
{
    T result = lhs;
	lhs += static_cast<T>(1);
    return result;
}

template <class T>
constexpr auto operator--(T& lhs) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T&>
{
	return lhs -= static_cast<T>(1);
}

template <class T>
constexpr auto operator--(T& lhs, int) ->
    std::enable_if_t<adobe::implementation::has_enabled_arithmetic<T>, T>
{
    T result = lhs;
	lhs -= static_cast<T>(1);
    return result;
}

/*************************************************************************************************/

#endif

/*************************************************************************************************/
