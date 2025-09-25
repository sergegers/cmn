# CMakeList.txt : CMake project for cmn, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set(CMAKE_CXX_STANDARD 23)
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

add_compile_definitions("$<$<CONFIG:Debug>:CMN_STATIC_TEST>")

project ("cmn")

#set(Boost_DEBUG ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

find_package(Boost REQUIRED COMPONENTS 
    system
    exception
    serialization
    unit_test_framework 
)

# message(STATUS "Boost_INCLUDE_DIRS = " ${Boost_INCLUDE_DIRS})
# message(STATUS "Boost_LIBRARY_DIRS = " ${Boost_LIBRARY_DIRS})

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# Add source to this project's executable.
add_library (
 cmn 
 STATIC

 "cmn/meta/macro.h"
 "cmn/meta/type_traits.h"
 "cmn/meta/function_traits.h"

"cmn/meta/boost/fusion/concepts.hpp"
"cmn/meta/boost/fusion/type_traits.hpp"
"cmn/meta/boost/mp11/concepts.hpp"
"cmn/meta/boost/mp11/type_traits.hpp"
"cmn/meta/boost/spirit/ext.hpp"
 
 "cmn/util/symbols.h"

 "cmn/util/fixed_string.h"
 "cmn/util/lexical_cast.h"
 "cmn/util/lexical_cast.cpp"
 "cmn/util/inverse.h" 
 "cmn/util/feature.h"
 
"cmn/io/manip/slot/fwd.h"
 "cmn/io/manip/slot/param.h"
 "cmn/io/manip/slot/decoder.h"
 "cmn/io/manip/slot/forwarder.h"
 "cmn/io/manip/slot/traits.h"
 "cmn/io/manip/slot/storage.h"
 "cmn/io/manip/slot/param.h"

"cmn/io/format/mix/parse_arg.h"
"cmn/io/format/mix/mixins.h"
"cmn/io/format/mix.h"
"cmn/io/format/context_traits.h"
"cmn/io/format/util.h"
"cmn/io/format/list.h"
"cmn/io/format.h"

 "cmn/range/io/format.h"
 "cmn/range/io/io.h"
 "cmn/range/io/manip.h"
 "cmn/range/io.h"
 
 "cmn/tuple/tuple.h"
 "cmn/tuple/op.h"
 "cmn/tuple/io/manip.h"
 "cmn/tuple/io/format.h"
 "cmn/tuple/io.h"
 
 "cmn/tuple/serialization.h"
 "cmn/tuple/va/concepts.h"
 "cmn/tuple/va/tuple.h"
 "cmn/tuple/va/tuple_view.h"
 "cmn/tuple/va/event.h"
 "cmn/tuple/va/traits.h"
 "cmn/tuple/va/adapt/binding.h" 
 "cmn/tuple/va/adapt/std_tuple.h" 
 "cmn/tuple/va/adapt/fusion.h" 
 "cmn/tuple/va/adapt/mpl.h"
 "cmn/tuple/va/adapt.h"

 "cmn/enum/detail/magic_get.h"
 "cmn/enum/detail/qualified_name.h" 
 "cmn/enum/detail/name_info.h" 
 "cmn/enum/detail/record_info.h"  
 "cmn/enum/detail/group_info.h" 
 "cmn/enum/detail/macro.h"  
 "cmn/enum/op.h" 
 "cmn/enum/traits.h" 
 "cmn/enum/enum.h" 
 "cmn/enum/bitfield.h" 
 "cmn/enum/combo.h" 
 "cmn/enum/io/format.h"
 "cmn/enum/io.h" 
 "cmn/enum/io/printer.h" 
 "cmn/enum/io/manip.h" 
 "cmn/enum/io/reader.h" 
 "cmn/enum/io/in.h" 
 "cmn/enum/io/in.cpp"

 "cmn/strong_typedef.h"
 "cmn/strong_typedef/strong_typedef.h" 
 "cmn/strong_typedef/io/io.h" 
 "cmn/strong_typedef/io/io.cpp" 
 "cmn/strong_typedef/io/format.h"
 "cmn/strong_typedef/io.h" 

 "cmn/algorithm/detail/result.h"
 "cmn/algorithm/detail/visitor.h"
 "cmn/algorithm/switch/ordered.h"
 "cmn/algorithm/switch/sequenced.h"
 "cmn/algorithm/switch/unordered.h"
 "cmn/algorithm/switch.h"
 "cmn/algorithm/at.h"
 "cmn/algorithm/binary_find.h"
 "cmn/algorithm/copy.h"
 "cmn/algorithm/erase.h"
 "cmn/algorithm/find.h"
 "cmn/algorithm/fold.h"
 "cmn/algorithm/for_each.h"
 "cmn/algorithm/rank.h"
 "cmn/algorithm/while.h"

 "cmn/error/exception.h"
 "cmn/error/exception.cpp"
 )

 set_target_properties(cmn PROPERTIES DEBUG_POSTFIX "-d")
 set_target_properties(cmn PROPERTIES RELEASE_POSTFIX "-r")

set(cmn_NatvisPath "${CMAKE_CURRENT_SOURCE_DIR}/cmn/cmn.natvis")

target_include_directories(cmn PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_options(cmn PUBLIC /NATVIS:${cmn_NatvisPath})

 add_executable (
  test_cmn
  
  "test_cmn/main.cpp"
  "test_cmn/error.cpp"
  "test_cmn/algorithm.cpp"
  "test_cmn/enum/enum.cpp" 
  "test_cmn/enum/io.cpp"
  "test_cmn/strong_typedef.cpp" 
  "test_cmn/tuple/tuple.cpp"
  "test_cmn/tuple/va.cpp"
  "test_cmn/tuple/io.cpp"
  "test_cmn/io.cpp" 
  "test_cmn/meta.cpp"
  "test_cmn/test_accessor.h"
  "test_cmn/manip.cpp" 
  "test_cmn/range/io.cpp"
  "test_cmn/util/util.cpp" 
  "test_cmn/util/fixed_string.cpp" 
  "test_cmn/util/feature.cpp"
 )

target_include_directories(test_cmn PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
set_target_properties(test_cmn PROPERTIES LINK_DEPENDS ${cmn_NatvisPath})

target_link_libraries(
    test_cmn 
    
    ${Boost_LIBRARIES}
    cmn
)

add_test(test_cmn test_cmn)
enable_testing()
