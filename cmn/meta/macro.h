#pragma once

#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

//*****************************************************************************
//
// Variadic version
//
//*****************************************************************************
#define CMN_PP_VA_CAT(...)	BOOST_PP_SEQ_CAT(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#ifdef __RESHARPER__
#   define RS_PASS_BY_VALUE_ATTR   [[jetbrains::pass_by_value]]
#   define RS_GUARD_ATTR           [[jetbrains::guard]]
#else
#   define RS_PASS_BY_VALUE_ATTR
#   define RS_GUARD_ATTR
#endif

#ifdef _MSC_VER
#   define CMN_DISABLE_WARNING(w, ...)    \
        __pragma( warning (push)) \
        __pragma( warning (disable: w))    \
        __VA_ARGS__ \
        __pragma( warning (pop))
#else
#   define CMN_DISABLE_WARNING(w, ...)
#endif
///////////////////////////////////////////////////////////////////////////////
//
// CMN_ANONYMOUS_VARIABLE(str) introduces an identifier starting with
// str and ending with a number that varies with the line.
//
// https://github.com/facebook/folly/blob/master/folly/Preprocessor.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef CMN_ANONYMOUS_VARIABLE
#   define CMN_ANONYMOUS_VARIABLE(...) CMN_DISABLE_WARNING(4189, [[maybe_unused]] CMN_PP_VA_CAT(var_, __VA_ARGS__, __COUNTER__))
#endif
