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

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# message(STATUS "Boost_INCLUDE_DIRS = " ${Boost_INCLUDE_DIRS})
# message(STATUS "Boost_LIBRARY_DIRS = " ${Boost_LIBRARY_DIRS})

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# Add source to this project's executable.
add_library (
 cmn 

 "cmn/meta/concepts.h"
 "cmn/meta/promote.h"
 "cmn/meta/util.h"
 "cmn/meta/symbols.h"

 "cmn/util/fixed_string.h"
 "cmn/util/param.h"
 "cmn/util/lexical_cast.h"
 "cmn/util/lexical_cast.cpp"
 "cmn/util/util.h"

 "cmn/io/manip/slot/concepts.h"
 "cmn/io/manip/slot/decoder.h"
 "cmn/io/manip/slot/forwarder.h"
 "cmn/io/manip/slot/manip.h"
 "cmn/io/manip/slot/traits.h"
 "cmn/io/manip/slot/util.h"

 "cmn/enum/macro.h"  
 "cmn/enum/op.h" 
 "cmn/enum/util.h" 
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
 
 "cmn/error/exception.h"
 "cmn/error/exception.cpp"

 )

target_include_directories(cmn PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

 add_executable (
 test
  
  "test/main.cpp"
  "test/enum.cpp"
 )

target_include_directories(test PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(test ${Boost_LIBRARIES})
target_link_libraries(test cmn)

 # TODO: Add tests and install targets if needed.

