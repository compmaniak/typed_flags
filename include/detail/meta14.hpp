//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_META14_HPP_
#define _TFL_META14_HPP_

#include <cstddef>
#include <utility>

namespace tfl
{
namespace detail
{

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

} // namespace detail
} // namespace tfl

#endif
