# -*- cmake -*-

# - Find Apache Portable Runtime
# Find the APR includes and libraries
# This module defines
#  SVN_INCLUDE_DIR and APRUTIL_INCLUDE_DIR, where to find apr.h, etc.
#  SVN_LIBRARIES and APRUTIL_LIBRARIES, the libraries needed to use APR.
#  SVN_FOUND If false, do not try to use APR.
# also defined, but not for general use are


find_path (SVN_INCLUDE_DIR svn_client.h
	/usr/local/include/subversion-1
	/usr/include/subversion-1)

set (SVN_NAMES
	svn_subr-1
	svn_client-1
	svn_wc-1)

foreach (LIB ${SVN_NAMES})
	find_library (LIB_PATH_${LIB}
		NAMES ${LIB}
		PATHS /usr/lib /usr/local/lib)
	if (NOT LIB_PATH-NOTFOUND)
		message (STATUS "Found SVN lib: ${LIB_PATH_${LIB}}")
		list (APPEND SVN_LIBRARIES ${LIB_PATH_${LIB}})
	endif ()
endforeach ()

if (SVN_LIBRARY AND SVN_INCLUDE_DIR)
	set (SVN_LIBRARIES ${SVN_LIBRARY})
	set (SVN_FOUND "YES")
else ()
	set (SVN_FOUND "NO")
endif ()


if (SVN_FOUND)
	if (NOT SVN_FIND_QUIETLY)
		message (STATUS "Found SVN: ${SVN_LIBRARIES}")
	endif (NOT SVN_FIND_QUIETLY)
else ()
	if (SVN_FIND_REQUIRED)
		message (FATAL_ERROR "Could not find SVN library")
	endif ()
endif ()

mark_as_advanced (
	SVN_LIBRARY
	SVN_INCLUDE_DIR)
