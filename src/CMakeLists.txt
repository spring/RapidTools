
include_directories (
		${APRUTIL_INCLUDE_DIR}
		${ZLIB_INCLUDE_DIRS}
		${SVN_INCLUDE_DIRS}
	)

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
		TempFile.cpp
)


target_link_libraries (BuildRapid
		${ZLIB_LIBRARIES}
		${SVN_LIBRARIES}
		${APR_LIBRARIES}
	)

