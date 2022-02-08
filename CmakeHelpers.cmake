cmake_minimum_required(VERSION 3.0.2)

# VS Filters
function(group_folders)
	foreach(_source IN ITEMS ${ARGN})
		get_filename_component(_source_path "${_source}" PATH)
		file(RELATIVE_PATH _source_path_rel "${SRC_DIR}" "${_source_path}")
		string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
		source_group("${_group_path}" FILES "${_source}")
	endforeach()
endfunction(group_folders)

# Boost Helper
function(find_boost)
	set(Boost_USE_STATIC_LIBS ON)
	find_package(Boost REQUIRED COMPONENTS ${ARGN})
	if (NOT Boost_FOUND)
		message(FATAL_ERROR "Failed to find boost library")
	endif()
	set(Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE)
	SET(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIR} PARENT_SCOPE)
	include_directories(${Boost_INCLUDE_DIR})
endfunction(find_boost)
