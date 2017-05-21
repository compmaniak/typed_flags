//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_TYPED_FLAGS_HPP_
#define _TFL_TYPED_FLAGS_HPP_

#include "detail/flags_storage.hpp"
#include "detail/meta.hpp"
#include <functional>

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

namespace detail
{

template<typename D>
class typed_flags_facet
{
private:

    D& this_() noexcept {
        return static_cast<D&>(*this);
    }

    D const& this_() const noexcept {
        return static_cast<D const&>(*this);
    }

public:

#if __cplusplus > 201402L
    
    template<typename... T>
    bool none() const noexcept {
        return (... && (!this_().template test<T>()));
    }

    template<typename... T>
    bool all() const noexcept {
        return (... && ( this_().template test<T>()));
    }

    template<typename... T>
    void set(bool value = true) noexcept {
        (..., (this_().set_bit(D::template index<T>(), value)));
    }

    template<typename... T>
    void set(flag<T>... flags) noexcept {
        (..., (this_().set_bit(D::template index<T>(), flags)));
    }

    template<typename... T>
    void get(flag<T>&... flags) const noexcept {
        (..., (flags = this_().template test<T>()));
    }

    template<typename... T>
    void reset() noexcept {
        (..., (this_().set_bit(D::template index<T>(), false)));
    }

    template<typename... T>
    void flip() noexcept {
        (..., (this_().set_bit(D::template index<T>(), !this_().template test<T>())));
    }
    
#else

    template<typename... T>
    bool none() const noexcept {
        bool r = true;
        int _[] = {0, (r = r && !this_().template test<T>(), 0)...};
        (void)_;
        return r;
    }

    template<typename... T>
    bool all() const noexcept {
        bool r = true;
        int _[] = {0, (r = r &&  this_().template test<T>(), 0)...};
        (void)_;
        return r;
    }

    template<typename... T>
    void set(bool value = true) noexcept {
        int _[] = {0, (this_().set_bit(D::template index<T>(), value), 0)...};
        (void)_;
    }

    template<typename... T>
    void set(flag<T>... flags) noexcept {
        int _[] = {0, (this_().set_bit(D::template index<T>(), flags), 0)...};
        (void)_;
    }

    template<typename... T>
    void get(flag<T>&... flags) const noexcept {
        int _[] = {0, (flags = this_().template test<T>(), 0)...};
        (void)_;
    }

    template<typename... T>
    void reset() noexcept {
        int _[] = {0, (this_().set_bit(D::template index<T>(), false), 0)...};
        (void)_;
    }

    template<typename... T>
    void flip() noexcept {
        int _[] = {0, (this_().set_bit(D::template index<T>(), !this_().template test<T>()), 0)...};
        (void)_;
    }

#endif

};

} // namespace detail

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
class typed_flags: 
    private detail::flags_storage<sizeof...(Args)>,
    private detail::typed_flags_facet<typed_flags<Args...>>
{
    typedef typed_flags<Args...> this_type;
    typedef detail::flags_storage<sizeof...(Args)> parent_type;
    typedef detail::typed_flags_facet<this_type> facet_type;

    friend class detail::typed_flags_facet<this_type>;

    static_assert(detail::is_unique<Args...>::value, "Flag types are not unique.");

public:

    using parent_type::set;
    using parent_type::reset;
    using parent_type::flip;
    using parent_type::none;
    using parent_type::any;
    using parent_type::all;
    using parent_type::to_integral;
    using parent_type::to_string;

    using facet_type::none;
    using facet_type::all;
    using facet_type::set;
    using facet_type::get;
    using facet_type::reset;
    using facet_type::flip;

    //! @name Creation
    //! @{
    
    //!
    //! Sets all flags to zero.
    //!
    typed_flags() {};
    
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
    template<typename CharT, typename Traits = std::char_traits<CharT>>
    explicit typed_flags(const CharT* str, size_t n = -1, 
        CharT zero = CharT('0'), 
        CharT one  = CharT('1'))
        : parent_type(str, (n == size_t(-1) ? Traits::length(str) : n), zero, one)
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
#ifdef DOXYGEN_WORKAROUND
    template<typename... T>
    bool none() const noexcept;
#endif
    
    //!
    //! Checks that at least one of specified flags is set.
    //! @param T... flag types.
    //! @returns true if one of specified flags is set, false otherwise.
    //! @note Invoking any() without template parameters checks at least one of all flags is set.
    //!
    template<typename... T>
    bool any() const noexcept
    {
        return !this->template none<T...>();
    }
    
    //!
    //! Checks that every specified flag is set.
    //! @param T... flag types.
    //! @returns true if every specified flag is set, false otherwise.
    //! @note Invoking all() without template parameters checks all flags are equal to one.
    //!
#ifdef DOXYGEN_WORKAROUND
    template<typename... T>
    bool all() const noexcept;
#endif
    
    //!
    //! Extracts values of specified flags to variables.
    //! @param flag<T>&... flag variables to store result.
    //!
#ifdef DOXYGEN_WORKAROUND
    template<typename... T>
    void get(flag<T>&... flags) const noexcept;
#endif
    
    //! @}
    //! @name Capacity
    //! @{
        
    //!
    //! Get the number of flags.
    //! @returns typed_flags<Args...>::length
    //!
    static constexpr size_t size() noexcept
    {
        return sizeof...(Args);
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
        static_assert(getter::value < size(), "Index is not defined");
        return getter::value;
    }
    
    //! @}
    //! @name Modifiers
    //! @{
    
    //!
    //! Sets specified flags from flag variables.
    //! @param flag<T>... flag variables to load from.
    //!
#ifdef DOXYGEN_WORKAROUND
    template<typename... T>
    void set(flag<T>... flags) noexcept;
#endif
    
    //!
    //! Changes specified flags.
    //! @param T... flag types.
    //! @param value sets flags to this value.
    //!
#ifdef DOXYGEN_WORKAROUND
    template<typename... T>
    void set(bool value = true) noexcept;
#endif
    
    //!
    //! Unsets specified flags.
    //! @param T... flag types.
    //!
#ifdef DOXYGEN_WORKAROUND
    template<typename... T>
    void reset() noexcept;
#endif
    
    //!
    //! Reverts specified flags i.e. zeros becomes ones and vice versa.
    //! @param T... flag types.
    //!
#ifdef DOXYGEN_WORKAROUND
    template<typename... T>
    void flip() noexcept;
#endif
    
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
    template<typename CharT = char,
             typename Traits = std::char_traits<CharT>,
             typename Allocator = std::allocator<CharT>>
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
        this->bitwise(other, std::bit_and<>{});
        return *this;
    }
    
    this_type& operator |= (this_type const& other ) noexcept
    {
        this->bitwise(other, std::bit_or<>{});
        return *this;
    }
    
    this_type& operator ^= (this_type const& other ) noexcept
    {
        this->bitwise(other, std::bit_xor<>{});
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
