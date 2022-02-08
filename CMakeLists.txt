cmake_minimum_required(VERSION 3.0.2)

project("NetBridgeSolution")

if (UNIX)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif (UNIX)    

# Helpers
include(CmakeHelpers.cmake)

# SHARED
add_subdirectory(shared)

# SGXLIB
add_subdirectory(sgxclib)

# SERVER
add_subdirectory(server)

if (NOT UNIX)
    # CLIENT
    add_subdirectory(client)
endif (NOT UNIX)

# Test
#add_subdirectory(test)
