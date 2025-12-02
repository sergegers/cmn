#pragma once

//
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0259r0.pdf
// https://github.com/unterumarmung/fixed_string/blob/master/include/fixed_string.hpp#L1
//

/*
    Licensed under the MIT License <http://opensource.org/licenses/MIT>.
    SPDX-License-Identifier: MIT
    Copyright (c) 2020 - 2020 Daniil Dudkin.

    Permission is hereby  granted, free of charge, to any  person obtaining a copy
    of this software and associated  documentation files (the "Software"), to deal
    in the Software  without restriction, including without  limitation the rights
    to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
    copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
    IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
    FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
    AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
    LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <array>
#include <functional>
#include <iterator>
#include <ostream>
#include <string_view>
#include <string>
#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <ranges>

namespace cmn
{

template
<
      typename Char
    , std::size_t N_
    , typename CharTraits = std::char_traits<Char>
>
class basic_fixed_string // NOLINT(cppcoreguidelines-special-member-functions)
{
public:
    using storage_type = std::array<Char, N_>;

    // keep nonstatic members public to satisfy standard layout requirement
    storage_type    m_data{};

    using traits_type = CharTraits;
    using value_type = Char;
    using pointer = value_type *;
    using const_pointer = value_type const *;
    using reference = value_type &;
    using const_reference = value_type const &;
    using iterator = storage_type::iterator;
    using const_iterator = storage_type::const_iterator;
    using reverse_iterator = storage_type::reverse_iterator;
    using const_reverse_iterator = storage_type::const_reverse_iterator;
    using size_type = std::size_t;
    using difference_type = ptrdiff_t;
    using string_view_type = std::basic_string_view<value_type, traits_type>;

    static constexpr auto npos = string_view_type::npos;
private:

    template <std::random_access_iterator It, std::sentinel_for<It> Se>
        requires std::indirectly_copyable<It, iterator>

    constexpr auto init(It first, Se last) -> void
    {
        std::size_t const length = std::distance(first, last);
        if (length > N_)
            throw std::out_of_range{ "String is too long" };

        std::copy(first, last, std::begin(m_data));
        if (length < N_) m_data[length] = 0;
    }

    template <std::size_t M_> requires (M_ <= N_)
    constexpr auto init(value_type const (&array)[M_]) -> void
    {
        std::ranges::copy(array, std::ranges::begin(m_data));
        if constexpr (M_ < N_) m_data[M_] = 0;
    }

    constexpr auto sv() const -> string_view_type { return *this; }
public:
    ///////////////////////////////////////////////////////////////////////////////
    //
    // constructors
    //
    constexpr basic_fixed_string() noexcept = default;  // data() == nullptr, size() == 0

    constexpr explicit basic_fixed_string(Char c) noexcept:
        m_data{ c }
    {}

    constexpr basic_fixed_string(value_type const (&array)[N_]) // NOLINT(google-explicit-constructor)
        noexcept(std::copy_constructible<value_type>) requires (N_ > 0)
    {
        init(array);
    }

    //-----------------------------------------------------------------------------
    template <std::random_access_iterator It, std::sentinel_for<It> Se>
        requires std::indirectly_copyable<It, iterator>

    constexpr basic_fixed_string(It first, Se last)
    {
        init(first, last);
    }

    //-----------------------------------------------------------------------------
    //
    // interoperability
    //
    template <std::size_t M_> requires (M_ <= N_)
    explicit constexpr basic_fixed_string(basic_fixed_string<Char, M_, CharTraits> const &other)
        noexcept(std::copy_constructible<value_type>)
    {
        init(other.m_data);
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    // assignment operators
    //
    constexpr auto operator = (value_type const (&array)[N_]) noexcept(std::copy_constructible<value_type>)
        -> basic_fixed_string &
    {
        init(array);
        return *this;
    }

    //-----------------------------------------------------------------------------
    //
    // interoperability
    //
    template <std::size_t M_> requires (M_ <= N_)
    [[nodiscard]] explicit constexpr operator basic_fixed_string<Char, M_, CharTraits> () const
        noexcept(std::copy_constructible<value_type>)
    {
        return substr<0, M_>();
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    // iterators
    //
    [[nodiscard]] constexpr auto begin(this auto &self_) noexcept { return self_.m_data.begin(); }
    [[nodiscard]] constexpr auto end(this auto &self_) noexcept { return self_.m_data.end() - 1; }
    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator { return m_data.cbegin(); }
    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator { return m_data.cend() - 1; }
    [[nodiscard]] constexpr auto rbegin(this auto &self_) noexcept { return self_.m_data.rbegin() + 1; }
    [[nodiscard]] constexpr auto rend(this auto &self_) noexcept -> reverse_iterator { return self_.m_data.rend(); }
    [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator { return m_data.crbegin() + 1; }
    [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator { return m_data.crend(); }

    ///////////////////////////////////////////////////////////////////////////////
    //
    // capacity
    //
    [[nodiscard]] constexpr auto size() const noexcept -> size_type { return N_; }
    [[nodiscard]] constexpr auto length() const noexcept -> size_type { return size(); }
    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type { return size(); }
    [[nodiscard]] constexpr auto empty() const noexcept -> bool { return N_ == 0; }

    ///////////////////////////////////////////////////////////////////////////////
    //
    // element access
    //
    [[nodiscard]] constexpr auto &operator[](this auto &self_, size_type n) { return self_.m_data[n]; }
    [[nodiscard]] constexpr auto &at(this auto &self_, size_type n) { return self_.m_data.at(n); }

    [[nodiscard]] constexpr auto &front(this auto &self_) noexcept requires (!empty()) { return self_.m_data.front(); }
    [[nodiscard]] constexpr auto &back(this auto &self_) noexcept requires (!empty()) { return m_data.back(); }

    [[nodiscard]] constexpr auto data(this auto &self_) noexcept { return self_.m_data.data(); }
    [[nodiscard]] constexpr auto c_str() const noexcept -> const_pointer { return data(); }

  private:
    template <size_type M_>
    using same_with_other_size = basic_fixed_string<value_type, M_, traits_type>;

    template <size_type Pos_, size_type Count_, size_type Size_>
    constexpr static auto calculate_substr_size() -> size_type
    {
        if constexpr (Pos_ >= Size_)
            return 0;

        constexpr size_type rcount = std::min(Count_, Size_ - Pos_);

        return rcount;
    }

    template <size_type Pos_, size_type Count_>
    using substr_result_type = same_with_other_size<calculate_substr_size<Pos_, Count_, N_>()>;

  public:
    ///////////////////////////////////////////////////////////////////////////////
    //
    // string operations
    //
    [[nodiscard]] constexpr operator string_view_type() const noexcept // NOLINT(google-explicit-constructor)
    {
        return { data(), N_ };
    }

    // clang-format off
    template <size_type pos = 0, size_type count = npos> requires (pos <= N_)
    [[nodiscard]] constexpr auto substr() const noexcept -> substr_result_type<pos, count>
    // clang-format on
    {
        substr_result_type<pos, count> result;
        std::copy(begin() + pos, begin() + pos + result.size(), result.begin());
        return result;
    }

    constexpr auto copy(Char *dest, size_type count, size_type pos = 0) const -> size_type
    {
        auto const src_begin = m_data.begin() + pos;
        std::copy(src_begin, m_data.end(), dest);
        return std::distance(src_begin, m_data.end());
    }
    //-----------------------------------------------------------------------------
    template <size_type M_>
    [[nodiscard]] constexpr auto find(same_with_other_size<M_> const &str, size_type pos = 0) const noexcept -> size_type
    {
        if constexpr (M_ > N_)
            return npos;
        return sv().find(str.sv(), pos);
    }
    [[nodiscard]] constexpr auto find(string_view_type sv, size_type pos = 0) const noexcept -> size_type
    { return sv().find(sv, pos); }

    [[nodiscard]] constexpr auto find(value_type const *s, size_type pos, size_type n) const noexcept -> size_type
    { return sv().find(s, pos, n); }

    [[nodiscard]] constexpr auto find(value_type const *s, size_type pos = 0) const noexcept -> size_type
    { return sv().find(s, pos); }

    [[nodiscard]] constexpr auto find(value_type c, size_type pos = 0) const noexcept -> size_type
    { return sv().find(c, pos); }

    //-----------------------------------------------------------------------------
    template <size_type M_>
    [[nodiscard]] constexpr auto rfind(same_with_other_size<M_> const &str,
                                       size_type pos = npos) const noexcept -> size_type
    {
        if constexpr (M_ > N_)
            return npos;
        return sv().rfind(str.sv(), pos);
    }

    [[nodiscard]] constexpr auto rfind(string_view_type sv, size_type pos = npos) const noexcept -> size_type
    { return sv().rfind(sv, pos); }

    [[nodiscard]] constexpr auto rfind(value_type const *s, size_type pos, size_type n) const noexcept -> size_type
    { return sv().rfind(s, pos, n); }

    [[nodiscard]] constexpr auto rfind(value_type const *s, size_type pos = npos) const noexcept -> size_type
    { return sv().rfind(s, pos); }

    [[nodiscard]] constexpr auto rfind(value_type c, size_type pos = npos) const noexcept -> size_type
    { return sv().rfind(c, pos); }

    //-----------------------------------------------------------------------------
    template <size_type M_>
    [[nodiscard]] constexpr auto find_first_of(same_with_other_size<M_> const &str,
                                               size_type pos = 0) const noexcept -> size_type
    {
        if constexpr (M_ > N_)
            return npos;
        return sv().find_first_of(str.sv(), pos);
    }
    [[nodiscard]] constexpr auto find_first_of(string_view_type sv, size_type pos = 0) const noexcept -> size_type
    { return sv().find_first_of(sv, pos); }

    [[nodiscard]] constexpr auto find_first_of(value_type const *s, size_type pos, size_type n) const noexcept -> size_type
    { return sv().find_first_of(s, pos, n); }

    [[nodiscard]] constexpr auto find_first_of(value_type const *s, size_type pos = 0) const noexcept -> size_type
    { return sv().find_first_of(s, pos); }

    [[nodiscard]] constexpr auto find_first_of(value_type c, size_type pos = 0) const noexcept -> size_type
    { return sv().find_first_of(c, pos); }

    //-----------------------------------------------------------------------------
    template <size_type M_>
    [[nodiscard]] constexpr auto find_last_of(same_with_other_size<M_> const &str,
                                              size_type pos = npos) const noexcept -> size_type
    {
        if constexpr (M_ > N_)
            return npos;
        return sv().find_last_of(str.sv(), pos);
    }
    [[nodiscard]] constexpr auto find_last_of(string_view_type sv, size_type pos = npos) const noexcept -> size_type
    { return sv().find_last_of(sv, pos); }

    [[nodiscard]] constexpr auto find_last_of(value_type const *s, size_type pos, size_type n) const noexcept -> size_type
    { return sv().find_last_of(s, pos, n); }

    [[nodiscard]] constexpr auto find_last_of(value_type const *s, size_type pos = npos) const noexcept -> size_type
    { return sv().find_last_of(s, pos); }

    [[nodiscard]] constexpr auto find_last_of(value_type c, size_type pos = npos) const noexcept -> size_type
    { return sv().find_last_of(c, pos); }

    //-----------------------------------------------------------------------------
    template <size_type M_>
    [[nodiscard]] constexpr auto find_first_not_of(same_with_other_size<M_> const &str,
                                                   size_type pos = 0) const noexcept -> size_type
    {
        if constexpr (M_ > N_)
            return npos;
        return sv().find_first_of(str.sv(), pos);
    }
    [[nodiscard]] constexpr auto find_first_not_of(string_view_type sv, size_type pos = 0) const noexcept -> size_type
    { return sv().find_first_not_of(sv, pos); }

    [[nodiscard]] constexpr auto find_first_not_of(value_type const *s, size_type pos, size_type n) const noexcept -> size_type
    { return sv().find_first_not_of(s, pos, n); }

    [[nodiscard]] constexpr auto find_first_not_of(value_type const *s, size_type pos = 0) const noexcept -> size_type
    { return sv().find_first_not_of(s, pos); }

    [[nodiscard]] constexpr auto find_first_not_of(value_type c, size_type pos = 0) const noexcept -> size_type
    { return sv().find_first_not_of(c, pos); }

    //-----------------------------------------------------------------------------
    template <size_type M_>
    [[nodiscard]] constexpr auto find_last_not_of(same_with_other_size<M_> const &str,
                                                  size_type pos = npos) const noexcept -> size_type
    {
        if constexpr (M_ > N_)
            return npos;
        return sv().find_last_of(str.sv(), pos);
    }
    [[nodiscard]] constexpr auto find_last_not_of(string_view_type sv, size_type pos = npos) const noexcept -> size_type
    { return sv().find_last_not_of(sv, pos); }

    [[nodiscard]] constexpr auto find_last_not_of(value_type const *s, size_type pos, size_type n) const noexcept -> size_type
    { return sv().find_last_not_of(s, pos, n); }

    [[nodiscard]] constexpr auto find_last_not_of(value_type const *s, size_type pos = npos) const noexcept -> size_type
    { return sv().find_last_not_of(s, pos); }

    [[nodiscard]] constexpr auto find_last_not_of(value_type c, size_type pos = npos) const noexcept -> size_type
    { return sv().find_last_not_of(c, pos); }

    //-----------------------------------------------------------------------------
    [[nodiscard]] constexpr auto compare(string_view_type v) const noexcept -> int { return sv().compare(v); }
    [[nodiscard]] constexpr auto compare(size_type pos1, size_type count1, string_view_type v) const -> int
    { return sv().compare(pos1, count1, v); }

    [[nodiscard]] constexpr auto compare(size_type pos1, size_type count1, string_view_type v, size_type pos2,
                                         size_type count2) const noexcept -> int
    { return sv().compare(pos1, count1, v, pos2, count2); }

    [[nodiscard]] constexpr auto compare(value_type const *s) const noexcept -> int { return sv().compare(s); }

    [[nodiscard]] constexpr auto compare(size_type pos1, size_type count1, value_type const *s) const noexcept -> int
    { return sv().compare(pos1, count1, s); }

    [[nodiscard]] constexpr auto compare(size_type pos1, size_type count1, value_type const *s,
                                         size_type count2) const noexcept -> int
    { return sv().compare(pos1, count1, s, count2); }

    //-----------------------------------------------------------------------------
    //
    // compare, ignoring trailing zero
    //
    template <std::size_t M_>
    [[nodiscard]] constexpr auto compare_as_cstr(same_with_other_size<M_> const &other) const noexcept -> int
    {
        const_iterator lhs_it;
        typename same_with_other_size<M_>::const_iterator rhs_it;
        for (lhs_it = m_data.begin(), rhs_it = other.m_data.begin(); ; ++lhs_it, ++rhs_it)
        {
            auto const value = [](auto const &cont, auto it) constexpr -> int
            {
                return cont.end() == it? 0: *it;
            };

            auto const lhs = value(m_data, lhs_it);
            auto const rhs = value(other.m_data, rhs_it);
            if (lhs == 0 || rhs == 0) return lhs - rhs;
        }
    }

    [[nodiscard]] constexpr auto compare_as_cstr(string_view_type const &other) const noexcept -> int
    {
        const_iterator lhs_it;
        typename string_view_type::const_iterator rhs_it;
        for (lhs_it = m_data.begin(), rhs_it = other.begin(); ; ++lhs_it, ++rhs_it)
        {
            auto const value = [](auto const &cont, auto it) constexpr -> int
            {
                return cont.end() == it? 0: *it;
            };

            auto const lhs = value(m_data, lhs_it);
            auto const rhs = value(other, rhs_it);
            if (lhs == 0 || rhs == 0) return lhs - rhs;
        }
    }

    //-----------------------------------------------------------------------------
    [[nodiscard]] constexpr auto starts_with(string_view_type v) const noexcept -> bool
    { return sv().substr(0, v.size()) == v; }

    [[nodiscard]] constexpr auto starts_with(char c) const noexcept -> bool
    { return !empty() && traits_type::eq(front(), c); }

    [[nodiscard]] constexpr auto starts_with(value_type const *s) const noexcept -> bool
    { return starts_with(string_view_type(s)); }

    //-----------------------------------------------------------------------------
    [[nodiscard]] constexpr auto ends_with(string_view_type sv) const noexcept -> bool
    { return size() >= sv.size() && compare(size() - sv.size(), npos, sv) == 0; }

    [[nodiscard]] constexpr auto ends_with(value_type c) const noexcept -> bool
    { return !empty() && traits_type::eq(back(), c); }

    [[nodiscard]] constexpr auto ends_with(value_type const *s) const noexcept -> bool
    { return ends_with(string_view_type(s)); }

    //-----------------------------------------------------------------------------
    [[nodiscard]] constexpr auto contains(string_view_type sv) const noexcept -> bool { return find(sv) != npos; }
    [[nodiscard]] constexpr auto contains(value_type c) const noexcept -> bool { return find(c) != npos; }
    [[nodiscard]] constexpr auto contains(value_type const *s) const noexcept -> bool { return find(s) != npos; }

    //-----------------------------------------------------------------------------
    auto swap(basic_fixed_string &other) noexcept(std::is_nothrow_swappable_v<storage_type>) -> void
    { m_data.swap(other.m_data); }
};

template <typename Char, typename CharTraits, std::size_t N_>
auto swap(basic_fixed_string<Char, N_, CharTraits> &lhs,
          basic_fixed_string<Char, N_, CharTraits> &rhs) noexcept(noexcept(lhs.swap(rhs))) -> void
{
    lhs.swap(rhs);
}

///////////////////////////////////////////////////////////////////////////////
//
// equality
//
template <typename Char, typename CharTraits, std::size_t M1_, std::size_t M2_>
[[nodiscard]] constexpr auto operator == (basic_fixed_string<Char, M1_, CharTraits> const &lhs,
                                        basic_fixed_string<Char, M2_, CharTraits> const &rhs) -> bool
{
    if constexpr (M1_ != M2_)
        return false;
    using lhs_type = std::decay_t<decltype(lhs)>;
    using sv_type = lhs_type::string_view_type;
    return static_cast<sv_type>(lhs) == rhs;
}

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits, std::size_t N_>
[[nodiscard]] constexpr auto operator == (basic_fixed_string<Char, N_, CharTraits> const &lhs,
                                        std::basic_string_view<Char, CharTraits> rhs) -> bool
{
    using lhs_type = std::decay_t<decltype(lhs)>;
    using sv_type = lhs_type::string_view_type;
    return static_cast<sv_type>(lhs) == rhs;
}

template <typename Char, typename CharTraits, std::size_t N_>
[[nodiscard]] constexpr auto operator == (std::basic_string_view<Char, CharTraits> lhs,
                                        basic_fixed_string<Char, N_, CharTraits> const &rhs) -> bool
{
    using rhs_type = std::decay_t<decltype(rhs)>;
    using sv_type = rhs_type::string_view_type;
    return lhs == static_cast<sv_type>(rhs);
}

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits, std::size_t N_>
[[nodiscard]] constexpr auto operator == (basic_fixed_string<Char, N_, CharTraits> const &lhs,
                                        std::basic_string<Char, CharTraits> rhs) -> bool
{
    using lhs_type = std::decay_t<decltype(lhs)>;
    using sv_type = lhs_type::string_view_type;
    return static_cast<sv_type>(lhs) == rhs;
}

template <typename Char, typename CharTraits, std::size_t N_>
[[nodiscard]] constexpr auto operator == (std::basic_string<Char, CharTraits> lhs,
                                        basic_fixed_string<Char, N_, CharTraits> const &rhs) -> bool
{
    using rhs_type = std::decay_t<decltype(rhs)>;
    using sv_type = rhs_type::string_view_type;
    return lhs == static_cast<sv_type>(rhs);
}

///////////////////////////////////////////////////////////////////////////////
//
// comparisons
//
template <typename Char, typename CharTraits, std::size_t M1_, std::size_t M2_>
[[nodiscard]] constexpr auto operator <=> (basic_fixed_string<Char, M1_, CharTraits> const & lhs, basic_fixed_string<Char, M2_, CharTraits> const & rhs)
{
    using lhs_type = std::decay_t<decltype(lhs)>;
    using sv_type = lhs_type::string_view_type;
    return static_cast<sv_type>(lhs) <=> rhs;
}

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits, std::size_t N_>
[[nodiscard]] constexpr auto operator <=> (basic_fixed_string<Char, N_, CharTraits> const & lhs, std::basic_string_view<Char, CharTraits> rhs)
{
    using lhs_type = std::decay_t<decltype(lhs)>;
    using sv_type = lhs_type::string_view_type;
    return static_cast<sv_type>(lhs) <=> rhs;
}

template <typename Char, typename CharTraits, std::size_t N_>
[[nodiscard]] constexpr auto operator <=> (std::basic_string_view<Char, CharTraits> lhs, basic_fixed_string<Char, N_, CharTraits> const & rhs)
{
    using rhs_type = std::decay_t<decltype(rhs)>;
    using sv_type = rhs_type::string_view_type;
    return lhs <=> static_cast<sv_type>(rhs);
}

//-----------------------------------------------------------------------------
template <typename Char, typename CharTraits, std::size_t N_>
[[nodiscard]] constexpr auto operator <=> (basic_fixed_string<Char, N_, CharTraits> const & lhs, std::basic_string<Char, CharTraits> rhs)
{
    using lhs_type = std::decay_t<decltype(lhs)>;
    using sv_type = lhs_type::string_view_type;
    return static_cast<sv_type>(lhs) <=> rhs;
}

template <typename Char, typename CharTraits, std::size_t N_>
[[nodiscard]] constexpr auto operator <=> (std::basic_string<Char, CharTraits> lhs, basic_fixed_string<Char, N_, CharTraits> const & rhs)
{
    using rhs_type = std::decay_t<decltype(rhs)>;
    using sv_type = rhs_type::string_view_type;
    return lhs <=> static_cast<sv_type>(rhs);
}

///////////////////////////////////////////////////////////////////////////////
//
// CTAD
//
template <typename Char, std::size_t N_> basic_fixed_string(Char const (&)[N_]) -> basic_fixed_string<Char, N_>;

///////////////////////////////////////////////////////////////////////////////
template <std::size_t N_> using fixed_string = basic_fixed_string<char, N_>;
template <std::size_t N_> using fixed_u8string = basic_fixed_string<char8_t, N_>;
template <std::size_t N_> using fixed_u16string = basic_fixed_string<char16_t, N_>;
template <std::size_t N_> using fixed_u32string = basic_fixed_string<char32_t, N_>;
template <std::size_t N_> using fixed_wstring = basic_fixed_string<wchar_t, N_>;
template <std::size_t N_> using fixed_ustring = basic_fixed_string<unsigned char, N_>;

using empty_fixed_string = fixed_string<0>;
using empty_fixed_u8string = fixed_u8string<0>;
using empty_fixed_u16string = fixed_u16string<0>;
using empty_fixed_u32string = fixed_u32string<0>;
using empty_fixed_wstring = fixed_wstring<0>;
using empty_fixed_ustring = fixed_ustring<0>;

///////////////////////////////////////////////////////////////////////////////
//
// concatenation
//
template <typename Char, std::size_t N_, std::size_t M_, typename CharTraits>
constexpr auto operator + (basic_fixed_string<Char, N_, CharTraits> const &lhs, basic_fixed_string<Char, M_, CharTraits> const &rhs)
    -> basic_fixed_string<Char, N_ + M_, CharTraits>
{
    basic_fixed_string<Char, N_ + M_, CharTraits> result;
    std::copy(lhs.begin(), lhs.end(), result.begin());
    std::copy(rhs.begin(), rhs.end(), result.begin() + N_);
    return result;
}

template <typename Char, std::size_t N_, std::size_t M_, typename CharTraits>
constexpr auto operator + (Char const (&lhs)[N_], basic_fixed_string<Char, M_, CharTraits> const &rhs)
    -> basic_fixed_string<Char, N_ - 1 + M_, CharTraits>
{
    basic_fixed_string lhs2 = lhs;
    return lhs2 + rhs;
}

template <typename Char, std::size_t N_, std::size_t M_, typename CharTraits>
constexpr auto operator + (basic_fixed_string<Char, N_, CharTraits> const &lhs, Char const (&rhs)[M_])
    -> basic_fixed_string<Char, N_ + M_ - 1, CharTraits>
{
    basic_fixed_string rhs2 = rhs;
    return lhs + rhs2;
}

template <typename Char, std::size_t N_, typename CharTraits>
constexpr auto operator + (Char lhs, basic_fixed_string<Char, N_, CharTraits> const &rhs)
    -> basic_fixed_string<Char, N_ + 1, CharTraits>
{
    return from_char(lhs) + rhs;
}

template <typename Char, std::size_t N_, typename CharTraits>
constexpr auto operator + (basic_fixed_string<Char, N_, CharTraits> const &lhs, Char rhs)
    -> basic_fixed_string<Char, N_ + 1, CharTraits>
{
    return lhs + from_char(rhs);
}

///////////////////////////////////////////////////////////////////////////////
//
// conversion
//
///////////////////////////////////////////////////////////////////////////////
template <typename Char, std::size_t N_, typename CharTraits>
constexpr auto to_string(basic_fixed_string<Char, N_, CharTraits> const &str) noexcept ->  std::basic_string<Char, CharTraits>
{
    return str.data();
}

///////////////////////////////////////////////////////////////////////////////
//
// output
//
template <typename Char, std::size_t N_, typename CharTraits>
auto operator << (std::basic_ostream<Char, CharTraits> &out, basic_fixed_string<Char, N_, CharTraits> const &str)
    -> std::basic_ostream<Char, CharTraits> &
{
    out << str.data();
    return out;
}

///////////////////////////////////////////////////////////////////////////////
//
// literals
//
inline namespace literals
{

template <fixed_string Fs_> constexpr auto operator""_fs() noexcept
{
    // drop null for consistent behaviour with string_view
    return Fs_.template substr<0, (Fs_.size() > 0? Fs_.size() - 1: 0)>();
}

template <fixed_wstring Fs_> constexpr auto operator""_wfs() noexcept
{
    // drop null for consistent behaviour with string_view
    return Fs_.template substr<0, (Fs_.size() > 0? Fs_.size() - 1: 0)>();
}

}

} 

// hash support
template <typename Char, typename CharTraits, size_t N_>
struct std::hash<cmn::basic_fixed_string<Char, N_, CharTraits>>  // NOLINT(cert-dcl58-cpp)
{
    using argument_type = cmn::basic_fixed_string<Char, N_, CharTraits>;

    constexpr auto operator ()(argument_type const &str) const -> size_t
    {
        using sv_type = argument_type::string_view_type;
        return hash<sv_type>()(static_cast<sv_type>(str));
    }
};
