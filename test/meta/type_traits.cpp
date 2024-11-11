
#include <cmn/meta/type_traits.h>

namespace cmn
{

// TODO: enable

///// type_traits
//static_assert(std::is_same_v<add_const_on_t<int, char>, char>, "add_const_on failed 0");
//static_assert(std::is_same_v<add_const_on_t<int const, char>, char const>, "add_const_on failed 1");
//static_assert(std::is_same_v<add_const_on_t<int const, char, long>, long>, "add_const_on failed 2");
//static_assert(std::is_same_v<add_const_on_t<int, char, long>, char>, "add_const_on failed 3");
//static_assert(!std::is_same_v<add_const_on_t<int const, int &>, int const &>, "add_const_on failed 4");
//static_assert(!std::is_same_v<add_const_on_t<int const, int *>, int const *>, "add_const_on failed 5");
//static_assert(std::is_same_v<add_const_on_t<int const, int *>, int * const>, "add_const_on failed 6");
//static_assert(std::is_same_v<add_const_on_t<int const, char const &>, char const &>, "add_const_on failed 7");
//static_assert(std::is_same_v<add_const_on_t<int const, char const *>, char const * const>, "add_const_on failed 8");
//
//static_assert(std::is_same_v<deep_add_const_on_t<int, char>, char>, "add_const_on failed 0");
//static_assert(std::is_same_v<deep_add_const_on_t<int const, char>, char const>, "add_const_on failed 1");
//static_assert(std::is_same_v<deep_add_const_on_t<int const, char, long>, long>, "add_const_on failed 2");
//static_assert(std::is_same_v<deep_add_const_on_t<int, char, long>, char>, "add_const_on failed 3");
//static_assert(std::is_same_v<deep_add_const_on_t<int const, int &>, int const &>, "add_const_on failed 4");
//static_assert(std::is_same_v<deep_add_const_on_t<int const, int *>, int const * const>, "add_const_on failed 5");
//static_assert(!std::is_same_v<deep_add_const_on_t<int const, int *>, int * const>, "add_const_on failed 6");
//static_assert(std::is_same_v<deep_add_const_on_t<int const, char const &>, char const &>, "add_const_on failed 7");
//static_assert(std::is_same_v<deep_add_const_on_t<int const, char const *>, char const * const>, "add_const_on failed 8");
//
//static_assert(std::is_same_v<add_volatile_on_t<int, char>, char>, "add_volatile_on failed 0");
//static_assert(std::is_same_v<add_volatile_on_t<int volatile, char>, char volatile>, "add_volatile_on failed 1");
//
//static_assert(std::is_same_v<add_rvalue_reference_on_t<int, char>, char>, "add_rvalue_reference_on failed 0");
//static_assert(std::is_same_v<add_rvalue_reference_on_t<int &&, char>, char &&>, "add_rvalue_reference_on failed 1");
//
//static_assert(std::is_same_v<add_lvalue_reference_on_t<int, char>, char>, "add_lvalue_reference_on failed 0");
//static_assert(std::is_same_v<add_lvalue_reference_on_t<int &, char>, char &>, "add_lvalue_reference_on failed 1");
//static_assert(std::is_same_v<add_lvalue_reference_on_t<int &, char const>, char const &>, "add_lvalue_reference_on failed 2");
//
//static_assert(std::is_same_v<add_cvref_on_t<int, float>, float>, "add_cvr_on_t 0 failed");
//static_assert(std::is_same_v<add_cvref_on_t<int const, float>, float const>, "add_cvr_on_t 1 failed");
//static_assert(std::is_same_v<add_cvref_on_t<int volatile, float>, float volatile>, "add_cvr_on_t 2 failed");
//static_assert(std::is_same_v<add_cvref_on_t<int &, float>, float &>, "add_cvr_on_t 3 failed");
//static_assert(std::is_same_v<add_cvref_on_t<int &&, float>, float &&>, "add_cvr_on_t 4 failed");
//static_assert(std::is_same_v<add_cvref_on_t<int const &, float>, float const &>, "add_cvr_on_t 5 failed");
//static_assert(std::is_same_v<add_cvref_on_t<int volatile &&, float>, float volatile &&>, "add_cvr_on_t 6 failed");
//static_assert(std::is_same_v<add_cvref_on_t<int const volatile &, float>, float const volatile &>, "add_cvr_on_t 7 failed");
//static_assert(std::is_same_v<add_cvref_on_t<int volatile &&, float>, float volatile &&>, "add_cvr_on_t 8 failed");
//
//static_assert(std::is_same_v<remove_rvalue_reference_t<int &&>, int>, "remove_rvalue_reference_t 0 failed");          
//static_assert(std::is_same_v<remove_rvalue_reference_t<int &>, int &>, "remove_rvalue_reference_t 1 failed");
//static_assert(std::is_same_v<remove_rvalue_reference_t<int const &&>, int const>, "remove_rvalue_reference_t 2 failed");
//static_assert(std::is_same_v<remove_rvalue_reference_t<int const &>, int const &>, "remove_rvalue_reference_t 3 failed");
//
//static_assert(std::is_same_v<remove_lvalue_reference_t<int &&>, int &&>, "remove_lvalue_reference_t 0 failed");
//static_assert(std::is_same_v<remove_lvalue_reference_t<int &>, int>, "remove_rvalue_reference_t 1 failed");
//static_assert(std::is_same_v<remove_lvalue_reference_t<int const &&>, int const &&>, "remove_lvalue_reference_t 2 failed");
//static_assert(std::is_same_v<remove_lvalue_reference_t<int const &>, int const>, "remove_lvalue_reference_t 3 failed");
//
//static_assert(std::is_same_v<function::arg_type_c_t<void(int, char *, std::string const &), 0>, int>,
//    "arg_type_c - argument 0 failed");
//static_assert(std::is_same_v<function::arg_type_c_t<int(int, char *, std::string const &), 1>, char *>,
//    "arg_type_c - argument 1 failed");
//static_assert(std::is_same_v<function::arg_type_c_t<void(int, char *, std::string const &), 2>, std::string const &>,
//    "arg_type_c - argument 2 failed");
//
//// NOTE: migrate can_call -> std::is_invocable
//static_assert(std::is_invocable_v<decltype(::_itoa), int, char *, int>, "can_call -> std::is_invocable -> std::is_invocable 0 failed");
//static_assert(!std::is_invocable_v<decltype(::_itoa), int, char *>, "can_call -> std::is_invocable 1 failed");
//static_assert(std::is_invocable_v<decltype(::_itoa), int, char *, long>, "can_call -> std::is_invocable 2 failed");
//
//int g_i[2] { 1, 3 };
//
//int fn0(int const &, int const &, int const &);
//int fn1();
//
//static_assert(std::is_invocable_v<decltype(fn0), int, int, int>, "can_call -> std::is_invocable 3 failed");
//static_assert(!std::is_invocable_v<decltype(fn0)>, "can_call -> std::is_invocable 4 failed");
//static_assert(std::is_invocable_v<decltype(fn1)>, "can_call -> std::is_invocable 5 failed");
//
//struct ss
//{
//    static int fn0(int, char *, float) noexcept { return 0; }
//    static int fn1(int &, char *, float) noexcept { return 0; }
//    static auto fn2(int const &, char *, double) noexcept { return std::pair{ 1, "ab" }; }
//};
//
//static_assert(std::is_invocable_v<decltype(ss::fn0), int, char *, float>, "can_call -> std::is_invocable 6 failed");
//static_assert(!std::is_invocable_v<decltype(ss::fn0), char *, float, float>, "can_call -> std::is_invocable 7 failed");
//static_assert(std::is_invocable_v<decltype(ss::fn1), int &, char *, float>, "can_call -> std::is_invocable 8 failed");
//static_assert(!std::is_invocable_v<decltype(ss::fn1), int, char *, float>, "can_call -> std::is_invocable 9 failed");
//static_assert(!std::is_invocable_v<decltype(ss::fn1), char *, char *, float>, "can_call -> std::is_invocable 10 failed");
//static_assert(std::is_invocable_v<decltype(ss::fn2), int &, char *, double>, "can_call -> std::is_invocable 11 failed");
//static_assert(!std::is_invocable_v<decltype(ss::fn2), int, float, int(&)[2]>, "can_call -> std::is_invocable 12 failed");
//
//static_assert(carray_size_v<int[6]> == 6, "array_size failed 0");
//static_assert(carray_size_v<int[]> == 0, "array_size failed 1");
//static_assert(carray_size_v<int> == std::numeric_limits<std::size_t>::max(), "array_size failed 2");
//static_assert(carray_size_v<char> == std::numeric_limits<std::size_t>::max(), "array_size failed 3");
//
//static_assert(c::dereferenceable<int *>, "dereferenceable test 0 failed");
//static_assert(c::dereferenceable<int const *>, "dereferenceable test 1 failed");
//// BUG: VS 17.0.0 P5
//using test_ptr_t = struct test_ *;
//static_assert(c::dereferenceable<test_ptr_t>, "dereferenceable test 2 failed");
//static_assert(c::dereferenceable<std::shared_ptr<int>>, "dereferenceable test 3 failed");
//static_assert(c::dereferenceable<std::shared_ptr<int const>>, "dereferenceable test 4 failed");
//static_assert(c::dereferenceable<std::unique_ptr<float>>, "dereferenceable test 5 failed");
//static_assert(c::dereferenceable<std::vector<long>::iterator>, "dereferenceable test 6 failed");
//static_assert(c::dereferenceable<std::vector<int>::const_iterator>, "dereferenceable test 7 failed");
//static_assert(!c::dereferenceable<int>, "dereferenceable test 8 failed");
//static_assert(!c::dereferenceable<std::vector<int>>, "dereferenceable test 9 failed");
//
//static_assert(c::instance_of_any_integral<std::false_type>);
//static_assert(c::instance_of_bool<std::true_type>);


}
