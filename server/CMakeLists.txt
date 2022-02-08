cmake_minimum_required(VERSION 3.0.2)

set(PROJECT_NAME "NetBridgeServer")

project(${PROJECT_NAME})

# SET Server in debug mode for now
set(CMAKE_BUILD_TYPE Debug)

# SET OUTPUT BIN DIRECTORY
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/server)
MESSAGE(STATUS "Runtime output directory : " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# SGXCLIB
include_directories(${SGXLIB_INCLUDE_DIR})

# ADD SOURCE
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(
	GLOB_RECURSE
	SRC_LIST
	${SRC_DIR}/*
)

# ADD EXE
add_executable(${PROJECT_NAME} ${SRC_LIST})

# LINK LIBRARIES, pthread to fix ubuntu bug
if (UNIX)
target_link_libraries(${PROJECT_NAME} sgxlib shared pthread)
else()
target_link_libraries(${PROJECT_NAME} sgxlib shared)
endif (UNIX)

# GROUP FOLDERS
group_folders(${SRC_LIST})
