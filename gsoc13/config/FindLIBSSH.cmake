# - Find libssh
# Find the native LIBSSH headers and libraries.
#
#  LIBSSH_INCLUDE_DIRS - where to find libssh.h, etc.
#  LIBSSH_LIBRARIES    - List of libraries when using libssh.
#  LIBSSH_FOUND        - True if libssh found.
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# Look for the header file.
FIND_PATH(LIBSSH_INCLUDE_DIR NAMES libssh/libssh.h)

# Look for the library.
FIND_LIBRARY(LIBSSH_LIBRARY NAMES ssh libssh)

# handle the QUIETLY and REQUIRED arguments and set LIBSSH_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBSSH DEFAULT_MSG LIBSSH_LIBRARY LIBSSH_INCLUDE_DIR)

# Copy the results to the output variables.
IF(LIBSSH_FOUND)
  SET(LIBSSH_LIBRARIES ${LIBSSH_LIBRARY})
  SET(LIBSSH_INCLUDE_DIRS ${LIBSSH_INCLUDE_DIR})
ELSE(LIBSSH_FOUND)
  SET(LIBSSH_LIBRARIES)
  SET(LIBSSH_INCLUDE_DIRS)
ENDIF(LIBSSH_FOUND)

MARK_AS_ADVANCED(LIBSSH_INCLUDE_DIR LIBSSH_LIBRARY)
