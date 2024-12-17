#pragma once

#include <cmn/io/manip/slot/traits.h>

namespace cmn
{

struct test_accessor_t
{
    template <io::manip::c::slot_manipulator SlotManip>
    static auto decode(io::manip::decode_type_t<SlotManip> in) -> io::manip::keep_type_t<SlotManip>
    {
        using unswapped_keep_type = typename SlotManip::unswapped_keep_type;
        using keep_type = typename SlotManip::keep_type;

        return static_cast<keep_type>(unswapped_keep_type{ SlotManip::decoder.decode(in) });
    }

    template <io::manip::c::slot_manipulator SlotManip>
    static auto encode(io::manip::keep_type_t<SlotManip> out) -> io::manip::decode_type_t<SlotManip>
    {
        using unswapped_keep_type = typename SlotManip::unswapped_keep_type;
        using keep_type = typename SlotManip::keep_type;

        return SlotManip::decoder.encode(static_cast<keep_type>(unswapped_keep_type{ out }));
    }

};

}
