# Locate the glfw3 library

# GLFW_FOUND
# GLFW_INCLUDE_DIR
# GLFW_LIBRARY

FIND_PATH(GLFW3_INCLUDE_DIR 
	NAMES
		"GLFW/glfw3.h"
	PATHS
		"${CMAKE_SOURCE_DIR}/packages/glfw3/include"
	DOC
		"The directory where GLFW/glfw3.h resides"
)

FIND_LIBRARY(GLFW3_LIBRARY 
	NAMES
		glfw3
		glfw
	PATHS
		"${CMAKE_SOURCE_DIR}/packages/glfw3/lib-win"
		"${CMAKE_SOURCE_DIR}/packages/glfw3/lib-linux"
	DOC
		"The GLFW3 library"
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW3 DEFAULT_MSG
GLFW3_LIBRARY GLFW3_INCLUDE_DIR)

