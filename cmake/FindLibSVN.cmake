# -*- cmake -*-

# - Find Apache Portable Runtime
# Find the APR includes and libraries
# This module defines
#  SVN_INCLUDE_DIR and APRUTIL_INCLUDE_DIR, where to find apr.h, etc.
#  SVN_LIBRARIES and APRUTIL_LIBRARIES, the libraries needed to use APR.
#  SVN_FOUND If false, do not try to use APR.
# also defined, but not for general use are


FIND_PATH(SVN_INCLUDE_DIR svn_client.h
/usr/local/include/subversion-1
/usr/include/subversion-1
)

SET(SVN_NAMES
	svn_client-1
	svn_fs-1)

foreach(LIB ${SVN_NAMES})
	FIND_LIBRARY(LIB_PATH_${LIB}
		NAMES ${LIB}
		PATHS /usr/lib /usr/local/lib
	)
	if (NOT LIB_PATH-NOTFOUND)
		message(STATUS "Found SVN lib: ${LIB_PATH_${LIB}}")
		list(APPEND SVN_LIBRARIES ${LIB_PATH_${LIB}})
	endif()
endforeach()

IF (SVN_LIBRARY AND SVN_INCLUDE_DIR)
    SET(SVN_LIBRARIES ${SVN_LIBRARY})
    SET(SVN_FOUND "YES")
ELSE (SVN_LIBRARY AND SVN_INCLUDE_DIR)
  SET(SVN_FOUND "NO")
ENDIF (SVN_LIBRARY AND SVN_INCLUDE_DIR)


IF (SVN_FOUND)
   IF (NOT SVN_FIND_QUIETLY)
      MESSAGE(STATUS "Found SVN: ${SVN_LIBRARIES}")
   ENDIF (NOT SVN_FIND_QUIETLY)
ELSE (SVN_FOUND)
   IF (SVN_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find SVN library")
   ENDIF (SVN_FIND_REQUIRED)
ENDIF (SVN_FOUND)

MARK_AS_ADVANCED(
  SVN_LIBRARY
  SVN_INCLUDE_DIR
  )

