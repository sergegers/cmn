#pragma once

#include <utility>

#include <cmn/io/manip/format_options.h>
#include <cmn/strong_typedef/io/int_fmt.h>

namespace cmn::io
{

consteval auto get_format_options_info(c::unit auto)
{
    return std::pair{true, int_fmt_t::default_};
}

namespace manip
{

using int_fmt_storage_t = format_options_storage<int_fmt_t>;
extern template int_fmt_storage_t;

using int_fmt_slot_manip = format_options_manip<int_fmt_t>;
extern template int_fmt_slot_manip;

using int_fmt_forwarder = format_options_forwarder<int_fmt_t>;
extern template int_fmt_forwarder;

inline constexpr int_fmt_forwarder int_fmt{};

constexpr auto udec = int_fmt(int_fmt_t::dec);
constexpr auto uoct = int_fmt(int_fmt_t::oct);
constexpr auto uhex = int_fmt(int_fmt_t::hex);
constexpr auto ushowbase = int_fmt(int_fmt_t::showbase);
constexpr auto uhidebase = int_fmt(int_fmt_t::hidebase);
constexpr auto uupercase = int_fmt(int_fmt_t::uppercase);
constexpr auto ulowercase = int_fmt(int_fmt_t::lowercase);
constexpr auto uc = int_fmt(int_fmt_t::c);
constexpr auto uasm = int_fmt(int_fmt_t::asm_);
constexpr auto ulong_ = int_fmt(int_fmt_t::long_);
constexpr auto ushort_ = int_fmt(int_fmt_t::short_);
constexpr auto usign = int_fmt(int_fmt_t::sign);
constexpr auto unosign = int_fmt(int_fmt_t::nosign);
constexpr auto uforcesign = int_fmt(int_fmt_t::forcesign);

}

using manip::int_fmt;
using manip::uasm;
using manip::uc;
using manip::udec;
using manip::uforcesign;
using manip::uhex;
using manip::uhidebase;
using manip::ulong_;
using manip::ulowercase;
using manip::unosign;
using manip::ushort_;
using manip::ushowbase;
using manip::usign;
using manip::uupercase;

}
