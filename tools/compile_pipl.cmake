# Generate PiPL resources using the same Adobe Windows SDK sequence as Dithering MAJEED:
#   cl /EP source.r -> source.rr
#   PiPLtool source.rr -> source.rrc
#   cl /D MSWindows /EP source.rrc -> source.rc
if(NOT DEFINED PIPL_TOOL OR NOT DEFINED C_COMPILER OR NOT DEFINED AE_HEADERS
   OR NOT DEFINED PIPL_R OR NOT DEFINED PIPL_RR OR NOT DEFINED PIPL_RRC
   OR NOT DEFINED PIPL_RC)
  message(FATAL_ERROR "compile_pipl.cmake is missing a required PiPL argument")
endif()

foreach(required_file "${PIPL_TOOL}" "${C_COMPILER}" "${PIPL_R}")
  if(NOT EXISTS "${required_file}")
    message(FATAL_ERROR "Required PiPL input does not exist: ${required_file}")
  endif()
endforeach()
if(NOT EXISTS "${AE_HEADERS}/AE_Effect.h")
  message(FATAL_ERROR "Adobe AE SDK headers not found at: ${AE_HEADERS}")
endif()

get_filename_component(PIPL_DIR "${PIPL_RC}" DIRECTORY)
file(MAKE_DIRECTORY "${PIPL_DIR}")

set(PIPL_SCRIPT "${PIPL_DIR}/GenerateVHS6RIFTPiPL.cmd")
file(WRITE "${PIPL_SCRIPT}" [=[
@echo off
setlocal
set "CL_EXE=@CL_EXE@"
set "SDK_HEADERS=@SDK_HEADERS@"
set "PIPL_IN=@PIPL_IN@"
set "PIPL_RR=@PIPL_RR@"
set "PIPLTOOL=@PIPLTOOL@"
set "PIPL_RRC=@PIPL_RRC@"
set "PIPL_RC=@PIPL_RC@"

"%CL_EXE%" /nologo /I "%SDK_HEADERS%" /EP "%PIPL_IN%" > "%PIPL_RR%"
if errorlevel 1 exit /b %errorlevel%

"%PIPLTOOL%" "%PIPL_RR%" "%PIPL_RRC%"
if errorlevel 1 exit /b %errorlevel%

"%CL_EXE%" /nologo /D "MSWindows" /EP "%PIPL_RRC%" > "%PIPL_RC%"
if errorlevel 1 exit /b %errorlevel%

if not exist "%PIPL_RC%" exit /b 2
exit /b 0
]=])
file(READ "${PIPL_SCRIPT}" PIPL_SCRIPT_TEXT)
string(REPLACE "@CL_EXE@" "${C_COMPILER}" PIPL_SCRIPT_TEXT "${PIPL_SCRIPT_TEXT}")
string(REPLACE "@SDK_HEADERS@" "${AE_HEADERS}" PIPL_SCRIPT_TEXT "${PIPL_SCRIPT_TEXT}")
string(REPLACE "@PIPL_IN@" "${PIPL_R}" PIPL_SCRIPT_TEXT "${PIPL_SCRIPT_TEXT}")
string(REPLACE "@PIPL_RR@" "${PIPL_RR}" PIPL_SCRIPT_TEXT "${PIPL_SCRIPT_TEXT}")
string(REPLACE "@PIPLTOOL@" "${PIPL_TOOL}" PIPL_SCRIPT_TEXT "${PIPL_SCRIPT_TEXT}")
string(REPLACE "@PIPL_RRC@" "${PIPL_RRC}" PIPL_SCRIPT_TEXT "${PIPL_SCRIPT_TEXT}")
string(REPLACE "@PIPL_RC@" "${PIPL_RC}" PIPL_SCRIPT_TEXT "${PIPL_SCRIPT_TEXT}")
file(WRITE "${PIPL_SCRIPT}" "${PIPL_SCRIPT_TEXT}")

execute_process(
  COMMAND cmd.exe /C call "${PIPL_SCRIPT}"
  RESULT_VARIABLE PIPL_RESULT
  ERROR_VARIABLE PIPL_ERROR
)
if(NOT PIPL_RESULT EQUAL 0)
  message(FATAL_ERROR "Adobe PiPL generation failed (${PIPL_RESULT}): ${PIPL_ERROR}")
endif()
if(NOT EXISTS "${PIPL_RC}")
  message(FATAL_ERROR "PiPL generation did not create: ${PIPL_RC}")
endif()
message(STATUS "Generated PiPL resource: ${PIPL_RC}")
