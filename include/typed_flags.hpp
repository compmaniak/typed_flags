//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_TYPED_FLAGS_HPP_
#define _TFL_TYPED_FLAGS_HPP_

#include "detail/flags_storage.hpp"
#include "detail/meta.hpp"

namespace tfl
{

//!
//! @brief Single flag container.
//!
//! Simple typed wrapper around bool variable. Can be converted to/from bool.
//! @param T flag type.
//!
template<typename T>
class flag
{
    bool value;
    
public:

    //! @name Creation
    //! @{
        
    constexpr flag(): value(false)
    {}
    
    constexpr flag(bool v): value(v)
    {}
    
    //! @}
    //! @name Conversions
    //! @{
    
    constexpr operator bool () const noexcept
    {
        return value;
    }
    
    flag<T>& operator = (bool v) noexcept
    {
        value = v;
        return *this;
    }
    
    //! @}
};

//!
//! @brief Type-safe flag container.
//!
//! Templated frontend to raw bit storage. Allows type safe bit manipulations
//! translating user defined types to corresponding indexes.
//! @param Args... user defined types.
//!
//! @note Types can be incomplete.
//!
template<typename... Args>
class typed_flags: private detail::flags_storage<sizeof...(Args)>
{
    static_assert(detail::is_unique<Args...>::value, 
        "Flag types are not unique.");
            
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

    //! @name Creation
    //! @{
    
    //!
    //! Sets all flags to zero.
    //!
    typed_flags() = default;
    
    //!
    //! Sets concrete flags to corresponding values.
    //! @param flag<T>... flag values.
    //!
    template<typename... T>
    explicit typed_flags(flag<T>... flags) noexcept
    {
        set(flags...);
    }
    
    //!
    //! Loads flag values from integral number.<br> Least significant bit
    //! corresponds to the first parameter of the class template.
    //! Remaining flags are initialized to zeros.
    //! @param data source integral number.
    //!
    explicit typed_flags(unsigned long long data) noexcept
        : parent_type(data)
    {}
    
    //!
    //! Loads flag values from characters.<br> Rightmost character corresponds to 
    //! the first parameter of the class template. Remaining flags are initialized to zeros.
    //! @param str string used to initialize flags.
    //! @param n number of characters to read from string (optional).
    //! @param zero character representing unset flag (optional).
    //! @param one character representing set flag (optional).
    //! @throws std::invalid_argument if character is neither zero nor one.
    //!
    template<class CharT>
    explicit typed_flags(const CharT* str, typename 
        std::basic_string<CharT>::size_type n = std::basic_string<CharT>::npos,
        CharT zero = CharT('0'), CharT one = CharT('1'))
        : parent_type(n == std::basic_string<CharT>::npos 
                      ? detail::string_view<CharT>(str)
                      : detail::string_view<CharT>(str, n), zero, one)
    {}
    
    //! @}
    //! @name Element access
    //! @{
    
    //!
    //! Returns the value of the specified flag.
    //! @param T flag type.
    //! @returns true if the flag is set, false otherwise.
    //!
    template<typename T>
    bool test() const noexcept
    {
        return this->get_bit(index<T>());
    }
        
    //!
    //! Checks that every specified flag is unset.
    //! @param T... flag types.
    //! @returns true if every specified flag is unset, false otherwise.
    //! @note Invoking none() without template parameters checks all flags are equal to zero.
    //!
    template<typename... T>
    bool none() const noexcept
    {
        return (... && !this->get_bit(index<T>()));
    }
    
    //!
    //! Checks that at least one of specified flags is set.
    //! @param T... flag types.
    //! @returns true if one of specified flags is set, false otherwise.
    //! @note Invoking any() without template parameters checks at least one of all flags is set.
    //!
    template<typename... T>
    bool any() const noexcept
    {
        return !none<T...>();
    }
    
    //!
    //! Checks that every specified flag is set.
    //! @param T... flag types.
    //! @returns true if every specified flag is set, false otherwise.
    //! @note Invoking all() without template parameters checks all flags are equal to one.
    //!
    template<typename... T>
    bool all() const noexcept
    {
        return (... && this->get_bit(index<T>()));
    }
    
    //!
    //! Extracts values of specified flags to variables.
    //! @param flag<T>&... flag variables to store result.
    //!
    template<typename... T>
    void get(flag<T>&... flags) const noexcept
    {
        (..., (flags = this->get_bit(index<T>())));
    }
    
    //! @}
    //! @name Capacity
    //! @{
        
    //!
    //! Number of flags i.e. template parameters.
    //!
    static constexpr size_t length = sizeof...(Args);
    
    //!
    //! Get the number of flags.
    //! @returns typed_flags<Args...>::length
    //!
    constexpr size_t size() const noexcept
    {
        return length;
    }
    
    //!
    //! Get index of specified flag.
    //! @param T flag type.
    //! @returns size_t
    //!
    template<typename T>
    static constexpr size_t index() noexcept
    {
        using getter = detail::index_of<T, Args...>;
        static_assert(getter::value < length, "Index is not defined");
        return getter::value;
    }
    
    //! @}
    //! @name Modifiers
    //! @{
    
    //!
    //! Sets specified flags from flag variables.
    //! @param flag<T>... flag variables to load from.
    //!
    template<typename... T>
    void set(flag<T>... flags) noexcept
    {
        (..., this->set_bit(index<T>(), flags));
    }
    
    //!
    //! Changes specified flags.
    //! @param T... flag types.
    //! @param value sets flags to this value.
    //!
    template<typename... T>
    void set(bool value = true) noexcept
    {
        (..., this->set_bit(index<T>(), value));
    }
    
    //!
    //! Unsets specified flags.
    //! @param T... flag types.
    //!
    template<typename... T>
    void reset() noexcept
    {
        (..., this->set_bit(index<T>(), false));
    }
    
    //!
    //! Reverts specified flags i.e. zeros becomes ones and vice versa.
    //! @param T... flag types.
    //!
    template<typename... T>
    void flip() noexcept
    {
        (..., this->set_bit(index<T>(), !this->get_bit(index<T>())));
    }
    
    //! @}
    //! @name Conversions
    //! @{
    
    //!
    //! Converts flags to integral number.<br>
    //! Least significant bit corresponds to the first parameter of the class template.
    //! @param T target integral type.
    //! @returns integral number of type T.
    //! @note If type T can't hold all flags static assertion fails.
    //!
#ifdef DOXYGEN_WORKAROUND
    template<typename T>
    T to_integral() const noexcept;
#endif
    
    //!
    //! Converts flags to standard string.<br>
    //! Rightmost character corresponds to the first parameter of the class template.
    //! @param zero character representing unset flag (optional).
    //! @param one character representing set flag (optional).
    //! @returns std::basic_string<CharT, Traits, Allocator> 
    //!
#ifdef DOXYGEN_WORKAROUND
    template<class CharT = char,
             class Traits = std::char_traits<CharT>,
             class Allocator = std::allocator<CharT>> 
    auto to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const;
#endif
    
    //! @}    
    //! @name Logical member operators
    //! @{

    bool operator == (this_type const& other) const noexcept
    {
        return this->is_equal(other);
    }
    
    bool operator != (this_type const& other) const noexcept
    {
        return !this->is_equal(other);
    }
    
    //! @}
    //! @name Bitwise member operators
    //! @{
    
    this_type& operator &= (this_type const& other ) noexcept
    {
        this->template bitwise(other, [](auto x, auto y){
            return x & y;
        });
        return *this;
    }
    
    this_type& operator |= (this_type const& other ) noexcept
    {
        this->template bitwise(other, [](auto x, auto y){
            return x | y;
        });
        return *this;
    }
    
    this_type& operator ^= (this_type const& other ) noexcept
    {
        this->template bitwise(other, [](auto x, auto y){
            return x ^ y;
        });
        return *this;
    }
    
    this_type operator ~ () const noexcept
    {
        this_type tmp(*this);
        tmp.flip();
        return tmp;
    }
    
    //! @}
};

//! @name Bitwise non-member operators
//! @relates typed_flags
//! @{

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

//! @}

} // namespace tfl

#endif
