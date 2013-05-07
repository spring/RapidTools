add_library (Rapid
	ArchiveEntry.cpp
	BitArray.cpp
	Crc32.cpp
	FileEntry.cpp
	Gzip.cpp
	Hex.cpp
	Last.cpp
	Lua.cpp
	Marshal.cpp
	Md5.cpp
	PoolArchive.cpp
	PoolFile.cpp
	Store.cpp
	Svn.cpp
	TempFile.cpp
	Versions.cpp
	Zip.cpp
	ZipFile.cpp)

target_link_libraries (Rapid
	${APR_LIBRARIES}
	${APRUTIL_LIBRARIES}
	${LUA_LIBRARIES}
	${SVN_LIBRARIES}
	${ZIP_LIBRARIES}
	${ZLIB_LIBRARIES})
