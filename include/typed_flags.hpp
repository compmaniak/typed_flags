//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_TYPED_FLAGS_HPP_
#define _TFL_TYPED_FLAGS_HPP_

#include "typed_bitset.hpp"

namespace tfl
{
namespace detail
{

template<typename T, typename... Args>
struct typed_bitset_proxy;

template<size_t... I, typename... Args>
struct typed_bitset_proxy<
    std::index_sequence<I...>, Args...>
    : typed_bitset<
        layout<bit_index<Args, I>...>, sizeof...(Args)>
{
    using typed_bitset<
        layout<bit_index<Args, I>...>, sizeof...(Args)>::typed_bitset;
};

} // namespace detail

template<typename T>
using flag = bit<T>;

template<typename... Args>
struct typed_flags
    : detail::typed_bitset_proxy<
        std::make_index_sequence<sizeof...(Args)>, Args...>
{
    using detail::typed_bitset_proxy<
        std::make_index_sequence<sizeof...(Args)>, Args...>::typed_bitset_proxy;
    
    typed_flags<Args...> operator ~ () const noexcept
    {
        typed_flags<Args...> tmp(*this);
        tmp.flip();
        return tmp;
    }
};

} // namespace tfl

#endif
