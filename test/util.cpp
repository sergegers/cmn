
#include <cmn/util/util.h>
#include <cmn/util/fixed_string.h>

namespace cmn
{

using namespace literals;

static_assert(log10(1ul) == 1, "log10(1)");
static_assert(pow10(0ul) == 1, "pow10(0)");

static_assert("1"_fs == itoa<1>(), "1");
static_assert("123456"_fs == itoa<123456>(), "123456");
static_assert("-123456"_fs == itoa< -123456>(), "-123456");

static_assert(3 == bsf(0b1000u), "1000B");
static_assert(4 == bsf(0b1001'0000u), "10010000B");
static_assert(5 == bsf(0b10'0000u), "100000B");

static_assert(3 == bsr(0b1010u));
static_assert(7 == bsr(0b1001'0000u));
static_assert(5 == bsr(0b10'0000u));

static_assert
(
    std::is_same_v
    <
        index_swap_t<std::index_sequence<1, 2, 3, 4, 5>, 2, 4>,
        std::index_sequence<1, 4, 3, 2, 5>
    >,
    "swap_index_seq_t failed" 
);

}
