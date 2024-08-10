#pragma once

#include <string_view>

#include "types.h"

namespace cmn::name_
{

auto parse_enum_type(std::string_view enum_tname) -> enum_t;
auto parse_class_type(std::string_view enum_tname) -> class_t;
auto parse_type(std::string_view) -> type_t;

}
