//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_META_HPP_
#define _TFL_META_HPP_

#include <cstddef>
#include <utility>

namespace tfl::detail
{

constexpr size_t BAD_INDEX = -1;

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
struct name_index_sequence;

//
// Helper class counting occurrences of T in Args
//
template<typename T, typename... Args>
struct is_unique_one
{
    static constexpr bool value =
        (0 + ... + (T::index == Args::index)) == 1 &&
        (0 + ... + std::is_same<typename T::type, typename Args::type>::value) == 1;
};

//
// Helper class checking that every type is unique.
// Every type must appear in sequence only once.
//
template<typename... Args>
struct is_unique
{
    static constexpr bool value = (... && is_unique_one<Args, Args...>::value);
};

template<>
struct is_unique<empty>
{
    static constexpr bool value = true;
};

template<typename T, typename U>
struct index_getter;

//
// List is empty, so type index is BAD_INDEX
//
template<typename T>
struct index_getter<T, name_index_sequence<empty>>
{
    static constexpr size_t value = BAD_INDEX;
};

struct index
{
    size_t value = BAD_INDEX;
    
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
