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

project ("cmn")

#set(Boost_DEBUG ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/boost/)

find_package(Boost REQUIRED COMPONENTS 
    exception
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
 "cmn/meta/concepts.h"
 "cmn/meta/type_traits.h"
 "cmn/meta/function_traits.h"
 
 "cmn/meta/symbols.h"

 "cmn/util/fixed_string.h"
 "cmn/util/param.h"
 "cmn/util/lexical_cast.h"
 "cmn/util/lexical_cast.cpp"
 "cmn/util/inverse.h" 
 "cmn/util/strong_typedef.h" 
 "cmn/util/strong_typedef_io.h" 
 "cmn/util/strong_typedef_io.cpp" 

 "cmn/io/manip/slot/concepts.h"
 "cmn/io/manip/slot/decoder.h"
 "cmn/io/manip/slot/forwarder.h"
 "cmn/io/manip/slot/manip.h"
 "cmn/io/manip/slot/traits.h"

 "cmn/range/io.h"

 "cmn/tuple/tuple.h"
 "cmn/tuple/ops.h"
 "cmn/tuple/io.h"
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
 "cmn/tuple/va/detail/util.h"

 "cmn/enum/util/magic_get.h"
 "cmn/enum/util/qualified_name.h" 
 "cmn/enum/util/name_info.h" 
 "cmn/enum/util/record_info.h"  
 "cmn/enum/util/group_info.h" 
 "cmn/enum/util/groups_info.h"
 "cmn/enum/util/enum_info.h"
 "cmn/enum/util/macro.h"  
 "cmn/enum/util.h"
 "cmn/enum/op.h" 
 "cmn/enum/traits.h" 
 "cmn/enum/enum.h" 
 "cmn/enum/bitfield.h" 
 "cmn/enum/combo.h" 
 "cmn/enum/io.h" 
 "cmn/enum/print.h" 
 "cmn/enum/read.h" 
 "cmn/enum/read.cpp" 
 "cmn/enum/manip.h" 
 "cmn/enum/feature.h"
 
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
  test
  
  "test/main.cpp"
  "test/error.cpp"
  "test/algorithm.cpp"
  "test/enum.cpp" 
   "test/io.cpp" 
   "test/tuple/tuple.cpp"
   "test/tuple/va.cpp"
   "test/util.cpp" 
   "test/meta/type_traits.cpp"
 )

target_include_directories(test PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
set_target_properties(test PROPERTIES LINK_DEPENDS ${cmn_NatvisPath})

target_link_libraries(test ${Boost_LIBRARIES})
target_link_libraries(test cmn)

 # TODO: Add tests and install targets if needed.

