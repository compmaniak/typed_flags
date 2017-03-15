//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TYPED_FLAGS_HPP_
#define _TYPED_FLAGS_HPP_

#include "detail/flags_storage.hpp"
#include "detail/meta.hpp"
#include "detail/functional.hpp"

//
// Helper class for human-readable construction.
// Consider flag<foo>{1} or flag<bar>{0}
//
template<typename T>
struct flag
{
    flag(): value(false)
    {}
    
    flag(bool v): value(v)
    {}
    
    explicit operator bool () const noexcept
    {
        return value;
    }
    
    bool value;
};

//
// Templated frontend to storage implementation.
// It provides type safety and translates types to corresponding indexes.
//
template<typename... Args>
class typed_flags: private detail::flags_storage<sizeof...(Args)>
{
    static_assert(detail::is_unique<Args...>::value, 
        "Flag types are not unique.");
    
    typedef typename detail::to_name_index_sequence<
        std::make_index_sequence<sizeof...(Args)>,
        Args...>::type layout_type;
        
    typedef typed_flags<Args...> this_type;
    
    typedef detail::flags_storage<sizeof...(Args)> parent_type;
    
public:

    using parent_type::set;
    using parent_type::reset;
    using parent_type::flip;
    using parent_type::none;
    using parent_type::any;
    using parent_type::all;
    using parent_type::to_integral;
    using parent_type::to_string;
    
    static constexpr size_t length = sizeof...(Args);
    
    template<typename T>
    static constexpr size_t index() noexcept
    {
        return detail::get_index<T, layout_type>();
    }

    typed_flags() = default;
    
    template<typename... T>
    explicit typed_flags(flag<T>... flags) noexcept
    {
        set(flags...);
    }
    
    //
    // Forwarded constructors
    //
    explicit typed_flags(unsigned long long data) noexcept
        : parent_type(data)
    {}
    
    template<class CharT>
    explicit typed_flags(const CharT* str, typename 
        std::basic_string<CharT>::size_type n = std::basic_string<CharT>::npos,
        CharT zero = CharT('0'), CharT one = CharT('1'))
        : parent_type(n == std::basic_string<CharT>::npos 
                      ? detail::string_view<CharT>(str)
                      : detail::string_view<CharT>(str, n), zero, one)
    {}
    
    //
    // Element access 
    //
    template<typename T>
    bool test() const noexcept
    {
        return this->get_bit(index<T>());
    }
        
    template<typename... T>
    bool none() const noexcept
    {
        return (... && !this->get_bit(index<T>()));
    }
    
    template<typename... T>
    bool any() const noexcept
    {
        return !none<T...>();
    }
    
    template<typename... T>
    bool all() const noexcept
    {
        return (... && this->get_bit(index<T>()));
    }
    
    template<typename... T>
    void get(flag<T>&... flags) const noexcept
    {
        (..., (flags.value = this->get_bit(index<T>())));
    }
    
    //
    // Capacity
    //
    constexpr size_t size() const noexcept
    {
        return length;
    }
    
    //
    // Modifiers
    //
    template<typename... T>
    void set(flag<T>... flags) noexcept
    {
        (..., this->set_bit(index<T>(), flags.value));
    }
    
    template<typename... T>
    void set(bool value = true) noexcept
    {
        (..., this->set_bit(index<T>(), value));
    }
    
    template<typename... T>
    void reset() noexcept
    {
        (..., this->set_bit(index<T>(), false));
    }
    
    template<typename... T>
    void flip() noexcept
    {
        (..., this->set_bit(index<T>(), !this->get_bit(index<T>())));
    }

    //
    // Logical member operators
    //
    bool operator == (this_type const& other) const noexcept
    {
        return this->is_equal(other);
    }
    
    bool operator != (this_type const& other) const noexcept
    {
        return !this->is_equal(other);
    }
    
    //
    // Bitwise member operators
    //
    this_type& operator &= (this_type const& other ) noexcept
    {
        this->template bitwise<detail::bit_and>(other);
        return *this;
    }
    
    this_type& operator |= (this_type const& other ) noexcept
    {
        this->template bitwise<detail::bit_or>(other);
        return *this;
    }
    
    this_type& operator ^= (this_type const& other ) noexcept
    {
        this->template bitwise<detail::bit_xor>(other);
        return *this;
    }
    
    this_type operator ~ () const noexcept
    {
        this_type tmp(*this);
        tmp.flip();
        return tmp;
    }
};

//
// Non-member bitwise operators
//
template<typename... Args>
typed_flags<Args...> operator & ( typed_flags<Args...> const& lhs, typed_flags<Args...> const& rhs )
{
    typed_flags<Args...> res = lhs;
    return res &= rhs;
}

template<typename... Args>
typed_flags<Args...> operator | ( typed_flags<Args...> const& lhs, typed_flags<Args...> const& rhs )
{
    typed_flags<Args...> res = lhs;
    return res |= rhs;
}

template<typename... Args>
typed_flags<Args...> operator ^ ( typed_flags<Args...> const& lhs, typed_flags<Args...> const& rhs )
{
    typed_flags<Args...> res = lhs;
    return res ^= rhs;
}

#endif
