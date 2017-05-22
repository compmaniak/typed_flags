//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_META17_HPP_
#define _TFL_META17_HPP_

#include <cstddef>
#include <utility>

namespace tfl
{
namespace detail
{

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

} // namespace detail
} // namespace tfl

#endif
