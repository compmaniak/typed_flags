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

//
// Entity representing one flag with its type and index
//
template<typename T, size_t N>
struct name_index
{
    typedef T type;
    static constexpr size_t index = N;
};

//
// Sequence of name_index entities
//
template<typename... Args>
struct name_index_sequence;

template<typename T, typename... Args>
struct to_name_index_sequence;

template<size_t... I, typename... Args>
struct to_name_index_sequence<std::index_sequence<I...>, Args...>
{
    typedef name_index_sequence<name_index<Args, I>...> type;
};

//
// Helper class counting occurrences of T in Args
//
template<typename T, typename... Args>
struct counter
{
    static constexpr size_t value = (0 + ... + std::is_same<T, Args>::value);
};

//
// Helper class checking that every type is unique.
// Every type must appear in sequence only once.
//
template<typename... Args>
struct is_unique
{
    static constexpr bool value = (... && (counter<Args, Args...>::value == 1));
};

template<typename T, typename U>
struct index_getter;

struct index
{
    size_t value = -1;
    
    index() = default;
    
    constexpr explicit index(size_t v): value(v) {}
    
    constexpr explicit operator size_t() const { return value; }
};

constexpr index operator << (index const& lhs, index const& rhs)
{
    return lhs.value < rhs.value ? lhs : rhs;
}

//
// Find type by matching it with each element in the list
//
template<typename T, typename... Args>
struct index_getter<T, name_index_sequence<Args...>>
{
    static constexpr size_t value = static_cast<size_t>(
        (index{} << ... << (std::is_same<T, typename Args::type>
            ::value ? index{Args::index} : index{})));
};

} // namespace tfl::detail

#endif
