cmake_minimum_required(VERSION 3.25)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(GALAXY_SDK_DIR "${CMAKE_SOURCE_DIR}/dependencies/galaxy")

include(target_add_versioninfo)
include(galaxy)

project(zGogGalaxy
	DESCRIPTION "Union plugin for Gothic Games"
	VERSION 0.0.0.0
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC
    "src/**.cpp"
	"resources/*.h"
	"resources/*.rc"
)

add_library(${CMAKE_PROJECT_NAME} SHARED)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SRC})

target_include_directories(${CMAKE_PROJECT_NAME}
    INTERFACE
		"include/"

	PRIVATE
		BEFORE "userapi/"
		"src/"
		"resources/"
)

target_add_versioninfo(${CMAKE_PROJECT_NAME}
	VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}"
	COMPANY_NAME "Doick"
	PRODUCT_NAME "${CMAKE_PROJECT_NAME}"
)

add_subdirectory(dependencies)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE galaxy)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GALAXY_SDK_DIR}/lib/Galaxy.dll"
        "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/Galaxy.dll"
)