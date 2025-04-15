cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)
set(CMAKE_CXX_STANDARD 17)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (node-lua)

add_definitions(-DNAPI_VERSION=8)

set(LUA_PKG_NAME "luajit" CACHE STRING "The name of the pkg-config module for Lua")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED IMPORTED_TARGET GLOBAL ${LUA_PKG_NAME})

include_directories(${CMAKE_JS_INC})

file(GLOB SOURCE_FILES "src/*.cc" "src/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME}
    ${CMAKE_JS_LIB}
    PkgConfig::LUA
)

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
    # Generate node.lib
    execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND node ${CMAKE_CURRENT_SOURCE_DIR}/generate-declaration.js
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating d.ts files..."
)
