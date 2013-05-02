cmake_minimum_required(VERSION 2.8)
project(BuildRapid)
include(FindPkgConfig)

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

find_package (ZLIB REQUIRED)
#pkg_check_modules (APRPKG REQUIRED apr-1 apr-util-1)

add_executable (BuildRapid
	BuildRapid.cpp
	Crc32.cpp
	Gzip.cpp
	Hex.cpp
	Marshal.cpp
	Md5.cpp
	PoolArchive.cpp
	PoolEntry.cpp
	PoolFile.cpp
	qtmd5.cpp
	Store.cpp
	TempFile.cpp)

include_directories (
	${ZLIB_INCLUDE_DIRS})

target_link_libraries (BuildRapid
	${ZLIB_LIBRARIES})
