cmake_minimum_required(VERSION 2.8)
project(BuildRapid)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

find_package(APR REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibSVN REQUIRED)

set (standard "-std=c++11")
set (warning "-pedantic -Wall -Wextra -Wshadow")
set (debug "-ggdb")
set (release "-DNDEBUG -O3 -flto")
set (symbol "-fno-rtti -fdata-sections -ffunction-sections -fvisibility=hidden")
set (linker_symbol "-Wl,--gc-sections")

set (CMAKE_CXX_FLAGS "${standard} ${warning}")
set (CMAKE_CXX_FLAGS_DEBUG "${debug}")
set (CMAKE_CXX_FLAGS_RELEASE "${release} ${symbol}")
set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${linker_symbol}")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${release} ${symbol} ${debug}")
set (CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${linker_symbol}")


add_subdirectory(src)
