#pragma once

#include <type_traits>

#include <cmn/meta/concepts.h>

#include <cmn/enum/combo.h>
#include <cmn/enum/traits.h>
#include <cmn/enum/util.h>

#include <boost/operators.hpp>

namespace cmn::io
{

//
// predefined integer formats
//
enum class radix_fmt_t
{
	dec = 1,													// default
	oct,														// o
	hex,														// x
	mask = dec | oct | hex
};

enum class base_fmt_t
{
	showbase = enum_::next_step(radix_fmt_t::mask),				// #
	hidebase = showbase + enum_::next_step(radix_fmt_t::mask),	// default
	mask = showbase | hidebase
};

enum class lang_fmt_t
{
	asm_ = enum_::next_step(base_fmt_t::mask),					// a
	c	 = asm_ + enum_::next_step(base_fmt_t::mask),			// default
	mask = asm_ | c
};

enum class width_fmt_t
{
	short_ = enum_::next_step(lang_fmt_t::mask),				// s, default
	long_ = short_ + enum_::next_step(lang_fmt_t::mask),		// l
	mask = short_ | long_
};

enum class case_fmt_t
{
	uppercase = enum_::next_step(width_fmt_t::mask),			// u
	lowercase = uppercase + enum_::next_step(width_fmt_t::mask),// default
	mask = uppercase | lowercase
};

enum class sign_fmt_t
{
	sign = enum_::next_step(case_fmt_t::mask),					// ( ) space for zero
	nosign = sign + enum_::next_step(case_fmt_t::mask),			// default
	forcesign = nosign + enum_::next_step(case_fmt_t::mask),	// (+) + for zero
	mask = sign | nosign | forcesign
};

//-----------------------------------------------------------------------------
consteval auto to_int_fmt(c::scoped_enum auto x)
{
    return static_cast<std::underlying_type_t<int_fmt_t>>(x);
}

//-----------------------------------------------------------------------------
//
// NOTE: zero value isn't used, so we can get mask from value
//
enum class int_fmt_t : short
{
	dec = to_int_fmt(radix_fmt_t::dec),				// default
	oct = to_int_fmt(radix_fmt_t::oct),				// o
	hex = to_int_fmt(radix_fmt_t::hex),				// x

	showbase = to_int_fmt(base_fmt_t::showbase),		// #
	hidebase = to_int_fmt(base_fmt_t::hidebase),		// default

	asm_ = to_int_fmt(lang_fmt_t::asm_),				// a
    c = to_int_fmt(lang_fmt_t::c),					// default

	short_ = to_int_fmt(width_fmt_t::short_),			// s, default
    long_ = to_int_fmt(width_fmt_t::long_),			// l

	uppercase = to_int_fmt(case_fmt_t::uppercase),	// u
	lowercase = to_int_fmt(case_fmt_t::lowercase),	// default

	sign = to_int_fmt(sign_fmt_t::sign),				// ( ) space for zero
    nosign = to_int_fmt(sign_fmt_t::nosign),			// default
    forcesign = to_int_fmt(sign_fmt_t::forcesign),	// (+) + for zero

	empty = 0b0000'0000'0000'0000,
	default_ = dec | hidebase | c | short_ | lowercase | nosign,	// must be synced with
	                                                                // strong_typedef_traits::default_
	// masks                            
	radix_mask = to_int_fmt(radix_fmt_t::mask),
	base_mask = to_int_fmt(base_fmt_t::mask),
	lang_mask = to_int_fmt(lang_fmt_t::mask),
    width_mask = to_int_fmt(width_fmt_t::mask),
	case_mask = to_int_fmt(case_fmt_t::mask),
	sign_mask = to_int_fmt(sign_fmt_t::mask),
	// predefined formats
	sshort_asm_up_hex = hex | showbase | asm_ | short_ | uppercase | sign,    // +1Bh
	long_asm_up_hex = hex | showbase | asm_ | long_ | uppercase | nosign,     // 0000001Bh
	long_c_up_hex = hex | showbase | c | long_ | uppercase | forcesign        // +0x0000001B
};

consteval auto adapt_enum_info(int_fmt_t en)
{
	using namespace cmn::enum_;
	using enum int_fmt_t;

	return enum_info
	(
		default_ops(en, kind_t::combo) | op_interoperable
		, group_::make<dec, oct, hex>()
		, group_::make<showbase, hidebase>()
		, group_::make<asm_, c>()
		, group_::make<short_, long_>()
		, group_::make<uppercase, lowercase>()
		, group_::make<sign, nosign, forcesign>()
	);
}

CMN_ENUM_INJECT_OPS()

static_assert(c::strong_bitfield<int_fmt_t>);
static_assert(!enum_::nullable_v<int_fmt_t>);

//////////////////////////////////////////////////////////////////////////////
//
// for debugging purposes
//
struct int_fmt_wrapper_t: boost::bitwise<int_fmt_wrapper_t, int_fmt_t>
{
    int_fmt_t m_fmt_opt;

	constexpr int_fmt_wrapper_t(int_fmt_t fmt_opt): m_fmt_opt{ fmt_opt } {}
	constexpr operator int_fmt_t () const noexcept { return m_fmt_opt; }

    constexpr auto operator |= (int_fmt_t rhs) -> int_fmt_wrapper_t & { return m_fmt_opt |= rhs, *this; }
    constexpr auto operator &= (int_fmt_t rhs) -> int_fmt_wrapper_t & { return m_fmt_opt &= rhs, *this; }
    constexpr auto operator ^= (int_fmt_t rhs) -> int_fmt_wrapper_t & { return m_fmt_opt ^= rhs, *this; }
};

}

namespace cmn::enum_::op
{

extern template auto operator << (std::ostream &, cmn::io::int_fmt_t) -> std::ostream &;
extern template auto operator << (std::wostream &, cmn::io::int_fmt_t) -> std::wostream &;

}
