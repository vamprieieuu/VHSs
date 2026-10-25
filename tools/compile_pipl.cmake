# Generate the Windows PiPL resource using Adobe's documented SDK sequence:
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
if(NOT EXISTS "${AE_HEADERS}/AEConfig.h")
  message(FATAL_ERROR "Adobe AE SDK headers not found at: ${AE_HEADERS}")
endif()

get_filename_component(PIPL_DIR "${PIPL_RC}" DIRECTORY)
file(MAKE_DIRECTORY "${PIPL_DIR}")

execute_process(
  COMMAND "${C_COMPILER}" /nologo /I "${AE_HEADERS}" /EP "${PIPL_R}"
  OUTPUT_FILE "${PIPL_RR}"
  RESULT_VARIABLE PREPROCESS_RESULT
  ERROR_VARIABLE PREPROCESS_ERROR
)
if(NOT PREPROCESS_RESULT EQUAL 0)
  message(FATAL_ERROR "PiPL source preprocessing failed (${PREPROCESS_RESULT}): ${PREPROCESS_ERROR}")
endif()

execute_process(
  COMMAND "${PIPL_TOOL}" "${PIPL_RR}" "${PIPL_RRC}"
  RESULT_VARIABLE PIPL_RESULT
  ERROR_VARIABLE PIPL_ERROR
)
if(NOT PIPL_RESULT EQUAL 0)
  message(FATAL_ERROR "PiPLtool failed (${PIPL_RESULT}): ${PIPL_ERROR}")
endif()
if(NOT EXISTS "${PIPL_RRC}")
  message(FATAL_ERROR "PiPLtool reported success but did not create: ${PIPL_RRC}")
endif()

execute_process(
  COMMAND "${C_COMPILER}" /nologo /D MSWindows /EP "${PIPL_RRC}"
  OUTPUT_FILE "${PIPL_RC}"
  RESULT_VARIABLE RC_RESULT
  ERROR_VARIABLE RC_ERROR
)
if(NOT RC_RESULT EQUAL 0)
  message(FATAL_ERROR "Final PiPL .rc preprocessing failed (${RC_RESULT}): ${RC_ERROR}")
endif()
if(NOT EXISTS "${PIPL_RC}")
  message(FATAL_ERROR "PiPL resource generation did not create: ${PIPL_RC}")
endif()

message(STATUS "Generated PiPL resource: ${PIPL_RC}")
