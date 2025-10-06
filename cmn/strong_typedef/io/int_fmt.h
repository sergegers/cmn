
#include <cmn/meta/concepts.h>
#include <cmn/enum/combo.h>

namespace cmn::io
{

//
// predefined integer formats
//
enum class radix_fmt_t
{
	dec = 0b0000'0000'0000'0001,    // default
	hex = 0b0000'0000'0000'0010,    // x
	mask = dec | hex
};

enum class base_fmt_t
{
	showbase = 0b0000'0000'0000'0100,    // #
	hidebase = 0b0000'0000'0000'1000,    // default
	mask = showbase | hidebase
};

enum class lang_fmt_t
{
	asm_ = 0b0000'0000'0001'0000,    // a
	c = 0b0000'0000'0010'0000,    // default
	mask = asm_ | c
};

enum class width_fmt_t
{
	short_ = 0b0000'0000'0100'0000,    // s, default
	long_ = 0b0000'0000'1000'0000,    // l
	mask = short_ | long_
};

enum class case_fmt_t
{
	uppercase = 0b0000'0001'0000'0000,    // u
	lowercase = 0b0000'0010'0000'0000,    // default
	mask = uppercase | lowercase
};

enum class sign_fmt_t
{
	sign = 0b0000'0100'0000'0000,    // ( ) space for zero
	nosign = 0b0000'1000'0000'0000,    // default
	forcesign = 0b0001'0000'0000'0000,    // (+) + for zero
	mask = sign | nosign | forcesign
};

//-----------------------------------------------------------------------------
//
// NOTE: zero value isn't used, so we can get mask from value
//
enum class int_fmt_t : short
{
	dec = radix_fmt_t::dec,         // default
	hex = radix_fmt_t::hex,         // x

	showbase = base_fmt_t::showbase,     // #
	hidebase = base_fmt_t::hidebase,     // default

	asm_ = lang_fmt_t::asm_,         // a
	c = lang_fmt_t::c,            // default

	short_ = width_fmt_t::short_,       // s, default
	long_ = width_fmt_t::long_,        // l

	uppercase = case_fmt_t::uppercase,    // u
	lowercase = case_fmt_t::lowercase,    // default

	sign = sign_fmt_t::sign,         // ( ) space for zero
	nosign = sign_fmt_t::nosign,       // default
	forcesign = sign_fmt_t::forcesign,    // (+) + for zero

	empty = 0b0000'0000'0000'0000,
	default_ = dec | hidebase | c | short_ | lowercase | nosign,      // must be synced with
	// strong_typedef_traits::default_

	// masks                            
	radix_mask = radix_fmt_t::mask,
	base_mask = base_fmt_t::mask,
	lang_mask = lang_fmt_t::mask,
	width_mask = width_fmt_t::mask,
	case_mask = case_fmt_t::mask,
	sign_mask = sign_fmt_t::mask,
	// predefined formats
	sshort_asm_up_hex = hex | showbase | asm_ | short_ | uppercase | sign,        // +1Bh
	long_asm_up_hex = hex | showbase | asm_ | long_ | uppercase | nosign,       // 0000001Bh
	long_c_up_hex = hex | showbase | c | long_ | uppercase | forcesign        // +0x0000001B
};

consteval auto adapt_enum_info(int_fmt_t en)
{
	using namespace cmn::enum_;
	using enum int_fmt_t;

	return enum_info
	(
		default_ops(en, kind_t::combo) | op_interoperable
		, group_::make<dec, hex>()
		, group_::make<showbase, hidebase>()
		, group_::make<asm_, c>()
		, group_::make<short_, long_>()
		, group_::make<uppercase, lowercase>()
		, group_::make<sign, nosign, forcesign>()
	);
}

CMN_ENUM_INJECT_OPS()

static_assert(c::strong_bitfield<int_fmt_t>);

// for debugging purposes
struct int_fmt_wrapper_t { int_fmt_t m_fmt_opt; };

}
