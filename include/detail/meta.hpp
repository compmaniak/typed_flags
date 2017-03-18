//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _META_HPP_
#define _META_HPP_

#include <cstddef>
#include <utility>

namespace detail
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

struct empty;

//
// Sequence of name_index entities
//
template<typename Head=empty, typename... Tail>
struct name_index_sequence
{
    static constexpr size_t length = 1 + sizeof...(Tail);
};

template<>
struct name_index_sequence<empty>
{
    static constexpr size_t length = 0;
};

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

//
// List is empty, so type index is -1
//
template<typename T>
struct index_getter<T, name_index_sequence<empty>>
{
    static constexpr size_t value = -1;
};

struct index
{
    size_t value = -1;
    
    index() = default;
    
    constexpr explicit index(size_t v): value(v) {}
    
    constexpr operator size_t() const { return value; }
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
    static constexpr size_t value = 
        (index{} << ... << (std::is_same<T, typename Args::type>::value 
            ? index{Args::index} : index{}));
};

//
// Helper function for getting type index with compile-time assertion
//
template<typename T, typename Seq>
constexpr size_t get_index()
{
    using getter = index_getter<T, Seq>;
    static_assert(getter::value < Seq::length, "Index is not defined");
    return getter::value;
}

} // namespace detail

#endif
