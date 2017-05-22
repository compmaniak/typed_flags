//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#ifndef _TFL_FACET14_HPP_
#define _TFL_FACET14_HPP_

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
};

} // namespace detail

#endif
