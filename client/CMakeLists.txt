cmake_minimum_required(VERSION 3.0.2)

set(PROJECT_NAME "NetBridge")

project(${PROJECT_NAME})

# SET OUTPUT BIN DIRECTORY
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/client)
MESSAGE(STATUS "Runtime output directory : " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# SGXCLIB
include_directories(${SGXLIB_INCLUDE_DIR})

# ADD QT
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${SRC_DIR}/ui)
set(CMAKE_AUTOUIC ON)
find_package(Qt5Widgets REQUIRED)
qt5_add_resources(RESOURCES ${RESOURCE_DIR}/resources.qrc)

# ADD SOURCE
file(
	GLOB_RECURSE
	SRC_LIST
	${SRC_DIR}/*
)

# ADD EXE
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_WIN32_WINNT=0x0501)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${RESOURCES} ${RESOURCE_DIR}/resource.rc)

# LINK LIBRARIES
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} Qt5::Widgets sgxlib shared)

# GROUP FOLDERS
group_folders(${SRC_LIST})