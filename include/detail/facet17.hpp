//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_FACET17_HPP_
#define _TFL_FACET17_HPP_

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
};

} // namespace detail

#endif
