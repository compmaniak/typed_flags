//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_META14_HPP_
#define _TFL_META14_HPP_

#include <cstddef>
#include <utility>

namespace tfl::detail
{

template<typename T, size_t N>
struct name_index;

struct empty;

template<typename Head=empty, typename... Tail>
struct name_index_sequence;

template<typename T, typename... Args>
struct to_name_index_sequence;

template<size_t... I, typename... Args>
struct to_name_index_sequence<std::index_sequence<I...>, Args...>
{
    typedef name_index_sequence<name_index<Args, I>...> type;
};

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

template<typename T, typename U>
struct index_getter;

template<typename T>
struct index_getter<T, name_index_sequence<empty>>
{
    static constexpr size_t value = -1;
};

template<typename T, size_t N, typename... Tail>
struct index_getter<T, name_index_sequence<name_index<T, N>, Tail...>>
{
    static constexpr size_t value = N;
};

template<typename T, typename U, size_t N, typename... Tail>
struct index_getter<T, name_index_sequence<name_index<U, N>, Tail...>>
{
    static constexpr size_t value = index_getter<T, name_index_sequence<Tail...>>::value;
};

} // namespace tfl::detail

#endif
