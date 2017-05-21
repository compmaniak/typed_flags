//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_META_HPP_
#define _TFL_META_HPP_

#include <cstddef>
#include <utility>

namespace tfl
{
namespace detail
{

#if __cplusplus > 201402L

template<typename T, typename... Args>
struct counter
{
    static constexpr size_t value = (0 + ... + std::is_same<T, Args>::value);
};

template<typename... Args>
struct is_unique
{
    static constexpr bool value = (... && (counter<Args, Args...>::value == 1));
};

enum index: size_t {};

constexpr index operator << (index lhs, index rhs)
{
    return lhs < rhs ? lhs : rhs;
}

template<typename T, typename U, typename... Args>
struct index_of_impl;

template<typename T, size_t... I, typename... Args>
struct index_of_impl<T, std::index_sequence<I...>, Args...>
{
    static constexpr size_t value = 
        (index(-1) << ... << index(std::is_same<T, Args>::value ? I : -1));
};
    
template<typename T, typename... Args>
struct index_of
{
    static constexpr size_t value = index_of_impl<
        T, std::index_sequence_for<Args...>, Args...>::value;
};

#else

struct empty;

template<typename T, typename Head=empty, typename... Tail>
struct counter
{
    static constexpr size_t value = std::is_same<T, Head>::value 
        + counter<T, Tail...>::value;
};

template<typename T>
struct counter<T, empty>
{
    static constexpr size_t value = 0;
};

template<typename Head=empty, typename... Tail>
struct is_unique
{
    static constexpr bool value = counter<Head, Tail...>::value == 0
        && is_unique<Tail...>::value;
};

template<>
struct is_unique<empty>
{
    static constexpr bool value = true;
};

template<typename T, size_t I, typename H=empty, typename... Args>
struct index_of_impl
{
    static constexpr size_t value = index_of_impl<T, I + 1, Args...>::value;
};

template<typename T, size_t I, typename... Args>
struct index_of_impl<T, I, T, Args...>
{
    static constexpr size_t value = I;
};

template<typename T, size_t I>
struct index_of_impl<T, I, empty>
{
    static constexpr size_t value = -1;
};

template<typename T, typename... Args>
struct index_of
{
    static constexpr size_t value = index_of_impl<T, 0, Args...>::value;
};

#endif

} // namespace detail
} // namespace tfl

#endif
