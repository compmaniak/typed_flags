//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_TYPED_BITSET_HPP_
#define _TFL_TYPED_BITSET_HPP_

#include "detail/flags_storage.hpp"
#include "detail/meta.hpp"

namespace tfl
{

//!
//! @brief Single bit container.
//!
//! Simple typed wrapper around bool variable. Can be converted to/from bool.
//! @param T bit type.
//!
template<typename T>
class bit
{
    bool value;
    
public:

    //! @name Creation
    //! @{
        
    constexpr bit(): value(false)
    {}
    
    constexpr bit(bool v): value(v)
    {}
    
    //! @}
    //! @name Conversions
    //! @{
    
    constexpr operator bool () const noexcept
    {
        return value;
    }
    
    bit<T>& operator = (bool v) noexcept
    {
        value = v;
        return *this;
    }
    
    //! @}
};

template<typename T, size_t N>
using bit_index = detail::name_index<T, N>;

template<typename... Args>
using layout = detail::name_index_sequence<Args...>;

template<typename T, size_t N>
class typed_bitset;

//!
//! @brief Type-safe bit container.
//!
//! Templated frontend to raw bit storage. Allows type safe bit manipulations
//! translating user defined types to corresponding indexes.
//! @param Args... user defined bit mask.
//!
//! @note Types can be incomplete.
//!
template<typename... Args, size_t N>
class typed_bitset<layout<Args...>, N>
    : private detail::flags_storage<N>
{
    static_assert(detail::is_unique<Args...>::value, 
        "Bit types or indexes are not unique.");
    
    typedef layout<Args...> layout_type;
        
    typedef typed_bitset<layout_type, N> this_type;
    
    typedef detail::flags_storage<N> parent_type;
    
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
    //! Sets all bits to zero.
    //!
    typed_bitset() = default;
    
    //!
    //! Sets concrete bits to corresponding values.
    //! @param flag<T>... bit values.
    //!
    template<typename... T>
    explicit typed_bitset(bit<T>... bits) noexcept
    {
        set(bits...);
    }
    
    //!
    //! Loads bit values from integral number.<br> Least significant bit
    //! corresponds to the first parameter of the class template.
    //! Remaining bits are initialized to zeros.
    //! @param data source integral number.
    //!
    explicit typed_bitset(unsigned long long data) noexcept
        : parent_type(data)
    {}
    
    //!
    //! Loads bit values from characters.<br> Rightmost character corresponds to 
    //! the first parameter of the class template. Remaining bits are initialized to zeros.
    //! @param str string used to initialize bits.
    //! @param n number of characters to read from string (optional).
    //! @param zero character representing unset bit (optional).
    //! @param one character representing set bit (optional).
    //! @throws std::invalid_argument if character is neither zero nor one.
    //!
    template<class CharT>
    explicit typed_bitset(const CharT* str, typename 
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
    //! Returns the value of the specified bit.
    //! @param T bit type.
    //! @returns true if the bit is set, false otherwise.
    //!
    template<typename T>
    bool test() const noexcept
    {
        return this->get_bit(index<T>());
    }
        
    //!
    //! Checks that every specified bit is unset.
    //! @param T... bit types.
    //! @returns true if every specified bit is unset, false otherwise.
    //! @note Invoking none() without template parameters checks all bits are equal to zero.
    //!
    template<typename... T>
    bool none() const noexcept
    {
        return (... && !this->get_bit(index<T>()));
    }
    
    //!
    //! Checks that at least one of specified bits is set.
    //! @param T... bit types.
    //! @returns true if one of specified bits is set, false otherwise.
    //! @note Invoking any() without template parameters checks at least one of all bits is set.
    //!
    template<typename... T>
    bool any() const noexcept
    {
        return !none<T...>();
    }
    
    //!
    //! Checks that every specified bit is set.
    //! @param T... bit types.
    //! @returns true if every specified bit is set, false otherwise.
    //! @note Invoking all() without template parameters checks all bits are equal to one.
    //!
    template<typename... T>
    bool all() const noexcept
    {
        return (... && this->get_bit(index<T>()));
    }
    
    //!
    //! Extracts values of specified bits to variables.
    //! @param bit<T>&... variables to store result.
    //!
    template<typename... T>
    void get(bit<T>&... bits) const noexcept
    {
        (..., (bits = this->get_bit(index<T>())));
    }
    
    //! @}
    //! @name Capacity
    //! @{
        
    //!
    //! Number of bits i.e. template parameters.
    //!
    static constexpr size_t length = N;
    
    //!
    //! Get the number of bits.
    //! @returns typed_bitset<Args...>::length
    //!
    constexpr size_t size() const noexcept
    {
        return length;
    }
    
    //!
    //! Get index of specified bit.
    //! @param T bit type.
    //! @returns size_t
    //!
    template<typename T>
    static constexpr size_t index() noexcept
    {
        using getter = detail::index_getter<T, layout_type>;
        static_assert(getter::value != detail::BAD_INDEX, "Index is not defined");
        static_assert(getter::value < N, "Index is out of range");
        return getter::value;
    }
    
    //! @}
    //! @name Modifiers
    //! @{
    
    //!
    //! Sets specified bits from variables.
    //! @param bit<T>... variables to load from.
    //!
    template<typename... T>
    void set(bit<T>... bits) noexcept
    {
        (..., this->set_bit(index<T>(), bits));
    }
    
    //!
    //! Changes specified bits.
    //! @param T... bit types.
    //! @param value sets bits to this value.
    //!
    template<typename... T>
    void set(bool value = true) noexcept
    {
        (..., this->set_bit(index<T>(), value));
    }
    
    //!
    //! Unsets specified bits.
    //! @param T... bit types.
    //!
    template<typename... T>
    void reset() noexcept
    {
        (..., this->set_bit(index<T>(), false));
    }
    
    //!
    //! Reverts specified bits i.e. zeros becomes ones and vice versa.
    //! @param T... bit types.
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
    //! Converts bits to integral number.<br>
    //! Least significant bit corresponds to the first parameter of the class template.
    //! @param T target integral type.
    //! @returns integral number of type T.
    //! @note If type T can't hold all bits static assertion fails.
    //!
#ifdef DOXYGEN_WORKAROUND
    template<typename T>
    T to_integral() const noexcept;
#endif
    
    //!
    //! Converts bits to standard string.<br>
    //! Rightmost character corresponds to the first parameter of the class template.
    //! @param zero character representing unset bit (optional).
    //! @param one character representing set bit (optional).
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

template<typename T, size_t N>
typed_bitset<T,N> operator & (typed_bitset<T,N> const& lhs, typed_bitset<T,N> const& rhs)
{
    typed_bitset<T,N> res = lhs;
    return res &= rhs;
}

template<typename T, size_t N>
typed_bitset<T,N> operator | (typed_bitset<T,N> const& lhs, typed_bitset<T,N> const& rhs)
{
    typed_bitset<T,N> res = lhs;
    return res |= rhs;
}

template<typename T, size_t N>
typed_bitset<T,N> operator ^ (typed_bitset<T,N> const& lhs, typed_bitset<T,N> const& rhs)
{
    typed_bitset<T,N> res = lhs;
    return res ^= rhs;
}

//! @}

} // namespace tfl

#endif
