cmake_minimum_required (VERSION 2.8)
project (RapidTools)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/")

find_package (Lua51 REQUIRED)
find_package (ZLIB REQUIRED)
find_package (Zip REQUIRED)

include_directories (
	External/libgit2/include
	External
	${LUA_INCLUDE_DIR}
	${ZIP_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

set (standard "-std=c++0x")
set (warning "-pedantic -Wall -Wextra -Wshadow -Wno-unused-variable")
set (debug "-ggdb")
set (release "-DNDEBUG -O3 -flto")
set (symbol "-fno-rtti -fdata-sections -ffunction-sections")
set (linker_symbol "-Wl,--gc-sections")

set (CMAKE_CXX_FLAGS "${standard} ${warning}")
set (CMAKE_CXX_FLAGS_DEBUG "${debug}")
set (CMAKE_CXX_FLAGS_RELEASE "${release} ${symbol}")
set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${linker_symbol}")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${release} ${symbol} ${debug}")
set (CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${linker_symbol}")

set(BUILD_CLAR OFF CACHE BOOL "" FORCE)
add_subdirectory (External/libgit2)

add_library (Rapid
	External/md5/md5.cpp
	Rapid/ArchiveEntry.cpp
	Rapid/BitArray.cpp
	Rapid/Crc32.cpp
	Rapid/FileEntry.cpp
	Rapid/Gzip.cpp
	Rapid/Hex.cpp
	Rapid/Last.cpp
	Rapid/LastGit.cpp
	Rapid/Lua.cpp
	Rapid/Marshal.cpp
	Rapid/Md5.cpp
	Rapid/Optional.cpp
	Rapid/PoolArchive.cpp
	Rapid/PoolFile.cpp
	Rapid/ScopeGuard.cpp
	Rapid/Store.cpp
	Rapid/String.cpp
	Rapid/TempFile.cpp
	Rapid/Versions.cpp
	Rapid/Zip.cpp
	Rapid/ZipFile.cpp)

target_link_libraries (Rapid
	git2
	${APR_LIBRARIES}
	${APRUTIL_LIBRARIES}
	${LUA_LIBRARIES}
	${ZIP_LIBRARIES}
	${ZLIB_LIBRARIES})

add_executable (AddZip Tools/AddZip.cpp)
target_link_libraries (AddZip Rapid)

add_executable (BuildGit Tools/BuildGit.cpp)
target_link_libraries (BuildGit Rapid)

add_executable (MakeZip Tools/MakeZip.cpp)
target_link_libraries (MakeZip Rapid)

add_executable (Streamer Tools/Streamer.cpp)
target_link_libraries (Streamer Rapid)

