#ifndef _FUNCTIONAL_HPP_
#define _FUNCTIONAL_HPP_

//
// Including <functional> for bitwise operations is overkill,
// so implement them manually
//

namespace detail
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

} // namespace detail

#endif
