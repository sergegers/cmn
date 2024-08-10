#pragma once

#include <string>

#include "types.h"

namespace cmn::name_
{

auto generate_enum_type_name(enum_t const & enum_type) -> std::string;
auto generate_type_name(type_t const &type) -> std::string;

}

