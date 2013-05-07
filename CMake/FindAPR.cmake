# -*- cmake -*-

# - Find Apache Portable Runtime
# Find the APR includes and libraries
# This module defines
#  APR_INCLUDE_DIR and APRUTIL_INCLUDE_DIR, where to find apr.h, etc.
#  APR_LIBRARIES and APRUTIL_LIBRARIES, the libraries needed to use APR.
#  APR_FOUND and APRUTIL_FOUND, If false, do not try to use APR.
# also defined, but not for general use are
#  APR_LIBRARY and APRUTIL_LIBRARY, where to find the APR library.

# APR first.

find_path (APR_INCLUDE_DIR apr.h
	/usr/local/include/apr-1
	/usr/local/include/apr-1.0
	/usr/include/apr-1
	/usr/include/apr-1.0)

set (APR_NAMES ${APR_NAMES} apr-1)
find_library (APR_LIBRARY
	NAMES ${APR_NAMES}
	PATHS /usr/lib /usr/local/lib)

if (APR_LIBRARY AND APR_INCLUDE_DIR)
	set (APR_LIBRARIES ${APR_LIBRARY})
	set (APR_FOUND "YES")
else ()
	set (APR_FOUND "NO")
endif ()


if (APR_FOUND)
	if (NOT APR_FIND_QUIETLY)
		message (STATUS "Found APR: ${APR_LIBRARIES}")
	endif ()
else (APR_FOUND)
	if (APR_FIND_REQUIRED)
		message (FATAL_ERROR "Could not find APR library")
	endif ()
endif ()

mark_as_advanced (
	APR_LIBRARY
	APR_INCLUDE_DIR)

# Next, APRUTIL.

find_path (APRUTIL_INCLUDE_DIR apu.h
	/usr/local/include/apr-1
	/usr/local/include/apr-1.0
	/usr/include/apr-1
	/usr/include/apr-1.0)

set (APRUTIL_NAMES ${APRUTIL_NAMES} aprutil-1)
find_library (APRUTIL_LIBRARY
	NAMES ${APRUTIL_NAMES}
	PATHS /usr/lib /usr/local/lib)

if (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)
	set (APRUTIL_LIBRARIES ${APRUTIL_LIBRARY})
	set (APRUTIL_FOUND "YES")
else ()
	set (APRUTIL_FOUND "NO")
endif ()


if (APRUTIL_FOUND)
	if (NOT APRUTIL_FIND_QUIETLY)
		message (STATUS "Found APRUTIL: ${APRUTIL_LIBRARIES}")
	endif ()
else (APRUTIL_FOUND)
	if (APRUTIL_FIND_REQUIRED)
		message (FATAL_ERROR "Could not find APRUTIL library")
	endif ()
endif ()

mark_as_advanced (
	APRUTIL_LIBRARY
	APRUTIL_INCLUDE_DIR)
