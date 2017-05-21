//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_FLAGS_STORAGE_HPP_
#define _TFL_FLAGS_STORAGE_HPP_

#include <cstddef>
#include <array>
#include <string>
#include <cstring>
#include <stdexcept>

namespace tfl
{
namespace detail
{

//
// Class storing bits in continuous array similar to std::bitset.
// Unlike std::bitset allocates less memory.
//
template<size_t N>
class flags_storage
{
    // Storage array element type
    typedef uint8_t bank_type;
    
    // Size of bank in bits
    static constexpr size_t bank_bits = sizeof(bank_type) * 8;
    
    // Number of elements in storage array
    static constexpr size_t bank_count = N / bank_bits + (N % bank_bits != 0);
    
    // Total size of storage in bytes
    static constexpr size_t storage_size = bank_count * sizeof(bank_type);
    
    // Bit mask for last bank
    static constexpr bank_type bank_mask = N % bank_bits != 0
                     ? bank_type(~(size_t(-1) << (N % bank_bits))) 
                     : bank_type(-1);
                     
    template<typename T>
    std::enable_if_t<storage_size == 0> init(T /*data*/) noexcept
    {}
    
    template<typename T>
    std::enable_if_t<0 < storage_size && storage_size <= sizeof(T)> init(T data) noexcept
    {
        memcpy(m_data.data(), &data, storage_size);
        m_data.back() &= bank_mask;
    }
    
    template<typename T>
    std::enable_if_t<sizeof(T) < storage_size> init(T data) noexcept
    {
        reset();
        memcpy(m_data.data(), &data, sizeof(T));
    }
    
public:

    flags_storage() noexcept
    {
        reset();
    }
    
    explicit flags_storage(unsigned long long data) noexcept
    {
        init(data);
    }
    
    template<class CharT>
    explicit flags_storage(CharT const* src, size_t n, CharT zero, CharT one)
    {
        reset();
        size_t i = 0;
        for (auto it = src + n - 1; i < n && i < N; --it, ++i)
            if (*it == one)
                set_bit(i, true);
            else if (*it != zero)
                throw std::invalid_argument("Char is not zero or one");
    }
    
    //
    // Modifiers
    //
    void set_bit(size_t n, bool value) noexcept
    {
        auto const mask = bank_type(1) << (n % bank_bits);
        if (value)
            m_data[n / bank_bits] |= mask;
        else
            m_data[n / bank_bits] &= ~mask;
    }
    
    void set() noexcept
    {        
        for (auto& bank : m_data)
            bank = bank_type(-1);
        m_data.back() &= bank_mask;
    }
    
    void reset() noexcept
    {
        for (auto& bank : m_data)
            bank = 0;
    }
    
    void flip() noexcept
    {
        for (auto& bank : m_data)
            bank = ~bank;
        m_data.back() &= bank_mask;
    }
    
    //
    // Element access
    //
    bool get_bit(size_t n) const noexcept
    {
        auto const mask = bank_type(1) << (n % bank_bits);
        return (m_data[n / bank_bits] & mask) > 0;
    }
        
    bool none() const noexcept
    {
        for (auto& bank : m_data)
            if (bank != 0)
                return false;
        return true;
    }
    
    bool any() const noexcept
    {
        return !none();
    }
    
    bool all() const noexcept
    {
        // TODO use 'constexpr if' after some time
        if (m_data.empty())
            return false;
        auto it = m_data.begin();
        for (auto last = m_data.end() - 1; it != last; ++it)
            if (*it != bank_type(-1))
                return false;
        return *it == bank_mask;
    }
    
    //
    // Conversions
    //
    template<typename T>
    T to_integral() const noexcept
    {
        static_assert(std::is_integral<T>::value, "T is not an intergal type");
        static_assert(sizeof(T) * 8 >= N, "T can't hold all flags");
        T res = 0;
        // TODO use 'constexpr if' after some time
        if (!m_data.empty())
            memcpy(&res, m_data.data(), m_data.size() * sizeof(bank_type));
        return res;
    }
    
    template<class CharT = char,
             class Traits = std::char_traits<CharT>,
             class Allocator = std::allocator<CharT>>
    auto to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const
    {
        std::basic_string<CharT, Traits, Allocator> res;
        res.reserve(N);
        for (size_t i = N; i > 0;)
            res += get_bit(--i) ? one : zero;
        return res;
    }
    
    //
    // Operators' implementation
    //
    bool is_equal(flags_storage<N> const& other) const noexcept
    {
        if (this == &other)
            return true;
        return m_data == other.m_data;
    }
    
    template<typename BinFn>
    void bitwise(flags_storage<N> const& other, BinFn&& fn) noexcept
    {
        auto it1 = m_data.begin();
        auto it2 = other.m_data.begin();
        for (auto last = m_data.end(); it1 != last; ++it1, ++it2)
            *it1 = static_cast<bank_type>(fn(*it1, *it2));
    }
        
private:
    
    std::array<bank_type, bank_count> m_data;
};

} // namespace detail
} // namespace tfl

#endif
