#pragma once

#ifdef __RESHARPER__
#   define RS_PASS_BY_VALUE_ATTR   [[jetbrains::pass_by_value]]
#   define RS_GUARD_ATTR           [[jetbrains::guard]]
#else
#   define RS_PASS_BY_VALUE_ATTR
#   define RS_GUARD_ATTR
#endif
