//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_FUNCTIONAL_HPP_
#define _TFL_FUNCTIONAL_HPP_

//
// Including <functional> for bitwise operations is overkill,
// so implement them manually
//

namespace tfl::detail
{

struct bit_or
{
    template<typename T>
    constexpr T operator () (T const& lhs, T const& rhs) const
    {
        return lhs | rhs; 
    }
};

struct bit_and
{
    template<typename T>
    constexpr T operator () (T const& lhs, T const& rhs) const
    {
        return lhs & rhs;
    }
};

struct bit_xor
{
    template<typename T>
    constexpr T operator () (T const& lhs, T const& rhs) const
    {
        return lhs ^ rhs;
    }
};

} // namespace tfl::detail

#endif
