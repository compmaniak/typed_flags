//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_META17_HPP_
#define _TFL_META17_HPP_

#include <cstddef>
#include <utility>

namespace tfl::detail
{

template<typename T, size_t N>
struct name_index;

template<typename... Args>
struct name_index_sequence;

template<typename T, typename... Args>
struct to_name_index_sequence;

template<size_t... I, typename... Args>
struct to_name_index_sequence<std::index_sequence<I...>, Args...>
{
    typedef name_index_sequence<name_index<Args, I>...> type;
};

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

template<typename T, typename U>
struct index_getter;

template<typename T, size_t... I, typename... Args>
struct index_getter<T, name_index_sequence<name_index<Args, I>...>>
{
    static constexpr size_t value = (index(-1) << ... << (
        std::is_same<T, Args>::value ? index(I) : index(-1)));
};

} // namespace tfl::detail

#endif
