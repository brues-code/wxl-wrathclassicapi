# wxl-wrathclassicapi: a WarcraftXL extension for the 3.3.5a (12340) client.
#
# Ships the WrathClassicAPI backported Lua APIs as a WXL extension. It is a thin
# front-end: the API implementations, offsets and Lua glue are the shared core,
# compiled in from a sibling WrathClassicAPI checkout via that repo's
# cmake/WrathClassicAPICore.cmake. This target adds only the WXL entry points and
# a WXL-backed Game::IHookHost.
#
# Two build-time dependencies, both resolved as siblings by default and neither
# linked at runtime:
#   * wxl-core       — for include/wxl/PluginApi.h (the extension ABI, header-only)
#   * WrathClassicAPI — for the shared core sources and its build recipe
#
# The client is a 32-bit process, so this configures Win32 and nothing else.

cmake_minimum_required(VERSION 3.20)
project(wxl-wrathclassicapi LANGUAGES CXX C)

# --- where WarcraftXL (wxl-core) is, for the extension ABI header --------------------------------
if(NOT WXL_CORE_DIR AND DEFINED ENV{WXL_CORE_DIR})
    set(WXL_CORE_DIR "$ENV{WXL_CORE_DIR}")
endif()
set(WXL_CORE_DIR "${WXL_CORE_DIR}" CACHE PATH "WarcraftXL checkout this extension is built against")
if(NOT WXL_CORE_DIR)
    get_filename_component(WXL_CORE_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/../wxl-core" ABSOLUTE)
    set(WXL_CORE_DIR "${WXL_CORE_DEFAULT}" CACHE PATH
        "WarcraftXL checkout this extension is built against" FORCE)
endif()
if(NOT EXISTS "${WXL_CORE_DIR}/include/wxl/PluginApi.h")
    message(FATAL_ERROR
        "WarcraftXL was not found at '${WXL_CORE_DIR}'.\n"
        "Point the build at a checkout with -DWXL_CORE_DIR=<path>, or set the WXL_CORE_DIR "
        "environment variable. The path must contain include/wxl/PluginApi.h.")
endif()
message(STATUS "WarcraftXL:     ${WXL_CORE_DIR}")

# --- where WrathClassicAPI is, for the shared core --------------------------------------------------
if(NOT WRATHCLASSICAPI_DIR AND DEFINED ENV{WRATHCLASSICAPI_DIR})
    set(WRATHCLASSICAPI_DIR "$ENV{WRATHCLASSICAPI_DIR}")
endif()
set(WRATHCLASSICAPI_DIR "${WRATHCLASSICAPI_DIR}" CACHE PATH "WrathClassicAPI checkout supplying the core")
if(NOT WRATHCLASSICAPI_DIR)
    get_filename_component(WRATHCLASSICAPI_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/../WrathClassicAPI" ABSOLUTE)
    set(WRATHCLASSICAPI_DIR "${WRATHCLASSICAPI_DEFAULT}" CACHE PATH
        "WrathClassicAPI checkout supplying the core" FORCE)
endif()
if(NOT EXISTS "${WRATHCLASSICAPI_DIR}/cmake/WrathClassicAPICore.cmake")
    message(FATAL_ERROR
        "WrathClassicAPI was not found at '${WRATHCLASSICAPI_DIR}'.\n"
        "Point the build at a checkout with -DWRATHCLASSICAPI_DIR=<path>, or set the "
        "WRATHCLASSICAPI_DIR environment variable. The path must contain "
        "cmake/WrathClassicAPICore.cmake.")
endif()
message(STATUS "WrathClassicAPI: ${WRATHCLASSICAPI_DIR}")

# --- toolchain -------------------------------------------------------------------------------------
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "The target client is a 32-bit process; configure with -A Win32.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static CRT, matching the core: the DLL loads inside Wow.exe, which has no reason to carry the
# toolset's redistributable, and a missing one is a loader failure at process start with no log.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# --- the extension ---------------------------------------------------------------------------------
include("${WRATHCLASSICAPI_DIR}/cmake/WrathClassicAPICore.cmake")

# This target's own front-end sources (entry points + the WXL hook host + module glue).
# CONFIGURE_DEPENDS re-globs at build time, so adding a file needs no edit here.
file(GLOB_RECURSE WXL_WCA_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(wxl-wrathclassicapi SHARED ${WXL_WCA_SRC})
set_target_properties(wxl-wrathclassicapi PROPERTIES OUTPUT_NAME "wxl-wrathclassicapi" PREFIX "")

# Pull in the shared core: WrathClassicAPI's src/*.cpp (minus its LichLoader entry unit), the
# generated embedded-addon header, tinycbor, the core include dirs and the version define.
wrathclassicapi_add_core(wxl-wrathclassicapi)

target_include_directories(wxl-wrathclassicapi PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${WXL_CORE_DIR}/include")   # the extension ABI, shared verbatim with the core

# WXL_EXTENSION is what makes PluginApi.h declare the two entry points as exports.
target_compile_definitions(wxl-wrathclassicapi PRIVATE WXL_EXTENSION)

# --- deployment ------------------------------------------------------------------------------------
# WarcraftXL discovers extensions as Extensions\<folder>\<folder>.dll under the client directory, so
# the folder name and the DLL name have to agree with the target name above.
set(CLIENT_PATH "" CACHE PATH "Client folder the built DLL is copied into after each build")
if(CLIENT_PATH)
    add_custom_command(TARGET wxl-wrathclassicapi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "${CLIENT_PATH}/Extensions/wxl-wrathclassicapi"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:wxl-wrathclassicapi>"
                "${CLIENT_PATH}/Extensions/wxl-wrathclassicapi/wxl-wrathclassicapi.dll"
        COMMENT "Deploy wxl-wrathclassicapi.dll -> ${CLIENT_PATH}/Extensions/wxl-wrathclassicapi")
endif()
