cmake_minimum_required(VERSION 3.24)
project(VHS6RIFT LANGUAGES C CXX RC)

if(NOT WIN32)
  message(FATAL_ERROR "VHS6RIFT currently targets Windows x64 / After Effects 23.x.")
endif()
if(NOT MSVC)
  message(FATAL_ERROR "Use Visual Studio 2022 / MSVC for the Windows PiPL build.")
endif()

# Configurable Adobe After Effects SDK discovery
if(NOT DEFINED AE_SDK_ROOT OR AE_SDK_ROOT STREQUAL "")
  if(DEFINED ENV{AE_SDK_ROOT} AND NOT "$ENV{AE_SDK_ROOT}" STREQUAL "")
    set(AE_SDK_ROOT "$ENV{AE_SDK_ROOT}" CACHE PATH "Root of Adobe After Effects SDK" FORCE)
  elseif(DEFINED ENV{AE_SDK_PATH} AND NOT "$ENV{AE_SDK_PATH}" STREQUAL "")
    set(AE_SDK_ROOT "$ENV{AE_SDK_PATH}" CACHE PATH "Root of Adobe After Effects SDK" FORCE)
  else()
    set(AE_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/extern/AfterEffectsSDK" CACHE PATH "Root of Adobe After Effects SDK")
  endif()
endif()
message(STATUS "Searching for Adobe AE SDK under: ${AE_SDK_ROOT}")

# Layout 1: <AE_SDK_ROOT>/Headers/AE_Effect.h
# Layout 2: <AE_SDK_ROOT>/Examples/Headers/AE_Effect.h
# Layout 3: Nested zip extraction search
if(EXISTS "${AE_SDK_ROOT}/Headers/AE_Effect.h")
  set(AE_INCLUDE_ROOT "${AE_SDK_ROOT}")
  set(AE_HEADERS "${AE_SDK_ROOT}/Headers")
elseif(EXISTS "${AE_SDK_ROOT}/Examples/Headers/AE_Effect.h")
  set(AE_INCLUDE_ROOT "${AE_SDK_ROOT}/Examples")
  set(AE_HEADERS "${AE_INCLUDE_ROOT}/Headers")
else()
  file(GLOB_RECURSE AE_EFFECT_H_CANDIDATES LIST_DIRECTORIES FALSE "${AE_SDK_ROOT}/*/AE_Effect.h")
  if(AE_EFFECT_H_CANDIDATES)
    list(GET AE_EFFECT_H_CANDIDATES 0 AE_EFFECT_H_PATH)
    get_filename_component(AE_HEADERS "${AE_EFFECT_H_PATH}" DIRECTORY)
    get_filename_component(AE_INCLUDE_ROOT "${AE_HEADERS}" DIRECTORY)
    message(STATUS "Found nested AE SDK headers at: ${AE_HEADERS}")
  else()
    message(FATAL_ERROR 
      "\n================================================================================\n"
      "Adobe After Effects SDK headers not found under: ${AE_SDK_ROOT}\n\n"
      "Supported folder layouts:\n"
      "  1. <AE_SDK_ROOT>/Headers/AE_Effect.h\n"
      "  2. <AE_SDK_ROOT>/Examples/Headers/AE_Effect.h\n"
      "  3. <AE_SDK_ROOT>/Adobe After Effects * SDK/Headers/AE_Effect.h\n\n"
      "Configure via environment variable AE_SDK_ROOT or CMake argument -DAE_SDK_ROOT=<path>\n"
      "See README.md for instructions on obtaining and configuring the licensed Adobe SDK.\n"
      "================================================================================\n"
    )
  endif()
endif()

# Validate required core headers
set(AE_REQUIRED_HEADERS AE_Effect.h AE_EffectCB.h AE_EffectCBSuites.h AE_EffectUI.h AE_Macros.h)
set(AE_MISSING_HEADERS "")
foreach(HEADER_NAME ${AE_REQUIRED_HEADERS})
  if(NOT EXISTS "${AE_HEADERS}/${HEADER_NAME}")
    list(APPEND AE_MISSING_HEADERS "${HEADER_NAME}")
  endif()
endforeach()

# Param_Utils.h is shipped in the SDK's Util directory (as in Dithering MAJEED),
# not in Headers. Validate its real SDK location separately.
if(NOT EXISTS "${AE_INCLUDE_ROOT}/Util/Param_Utils.h")
  list(APPEND AE_MISSING_HEADERS "Util/Param_Utils.h")
endif()

if(AE_MISSING_HEADERS)
  message(FATAL_ERROR 
    "\n================================================================================\n"
    "Incomplete Adobe AE SDK installation. Missing required headers in ${AE_HEADERS}:\n"
    "  ${AE_MISSING_HEADERS}\n"
    "Please provide a complete licensed Adobe After Effects SDK.\n"
    "================================================================================\n"
  )
endif()

# Locate PiPLtool.exe
set(PIPL_TOOL "${AE_SDK_ROOT}/Resources/PiPLtool.exe")
if(NOT EXISTS "${PIPL_TOOL}")
  set(PIPL_TOOL "${AE_INCLUDE_ROOT}/Resources/PiPLtool.exe")
endif()
if(NOT EXISTS "${PIPL_TOOL}")
  set(PIPL_TOOL "${AE_SDK_ROOT}/Examples/Resources/PiPLtool.exe")
endif()
if(NOT EXISTS "${PIPL_TOOL}")
  file(GLOB_RECURSE PIPL_TOOL_CANDIDATES LIST_DIRECTORIES FALSE "${AE_SDK_ROOT}/*/PiPLtool.exe")
  if(PIPL_TOOL_CANDIDATES)
    list(GET PIPL_TOOL_CANDIDATES 0 PIPL_TOOL)
  endif()
endif()
if(NOT EXISTS "${PIPL_TOOL}")
  message(FATAL_ERROR 
    "\n================================================================================\n"
    "PiPLtool.exe not found anywhere under: ${AE_SDK_ROOT}\n"
    "Add the PiPLtool.exe executable supplied with your licensed Adobe After Effects SDK.\n"
    "================================================================================\n"
  )
endif()
message(STATUS "Using AE Headers: ${AE_HEADERS}")
message(STATUS "Using PiPLtool:   ${PIPL_TOOL}")

set(PIPL_R "${CMAKE_CURRENT_SOURCE_DIR}/resources/VHS6RIFTPiPL.r")
set(PIPL_RR "${CMAKE_CURRENT_BINARY_DIR}/VHS6RIFTPiPL.rr")
set(PIPL_RRC "${CMAKE_CURRENT_BINARY_DIR}/VHS6RIFTPiPL.rrc")
set(PIPL_RC "${CMAKE_CURRENT_BINARY_DIR}/VHS6RIFTPiPL.rc")
add_custom_command(OUTPUT "${PIPL_RC}"
  COMMAND "${CMAKE_COMMAND}"
    "-DPIPL_TOOL=${PIPL_TOOL}"
    "-DC_COMPILER=${CMAKE_C_COMPILER}"
    "-DAE_HEADERS=${AE_HEADERS}"
    "-DPIPL_R=${PIPL_R}"
    "-DPIPL_RR=${PIPL_RR}"
    "-DPIPL_RRC=${PIPL_RRC}"
    "-DPIPL_RC=${PIPL_RC}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/tools/compile_pipl.cmake"
  DEPENDS "${PIPL_R}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/compile_pipl.cmake"
  COMMENT "Generating VHS6RIFT PiPL resource"
  VERBATIM)
add_custom_target(VHS6RIFT_PiPL DEPENDS "${PIPL_RC}")

# The current VHS6RIFT native source does not call the optional ntscrs_c bridge.
# Keep the public include path, but do not make the AEX build depend on an absent Cargo project.
add_library(VHS6RIFT MODULE src/VHS6RIFT.cpp "${PIPL_RC}")
add_dependencies(VHS6RIFT VHS6RIFT_PiPL)
target_include_directories(VHS6RIFT PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/extern/ntscrs_c/include"
  "${AE_HEADERS}"
  "${AE_HEADERS}/SP"
  "${AE_INCLUDE_ROOT}/Util"
  "${AE_INCLUDE_ROOT}/Examples/Headers"
)
target_compile_features(VHS6RIFT PRIVATE cxx_std_17)
target_compile_definitions(VHS6RIFT PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(VHS6RIFT PRIVATE /EHsc /W3 /permissive-)
if(WIN32)
  target_link_libraries(VHS6RIFT PRIVATE ws2_32 userenv bcrypt advapi32 ntdll)
endif()
set_target_properties(VHS6RIFT PROPERTIES PREFIX "" SUFFIX ".aex" OUTPUT_NAME "VHS6RIFT")
set_target_properties(VHS6RIFT PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS OFF)
