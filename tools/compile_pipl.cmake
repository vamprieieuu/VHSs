# Generate the Windows PiPL resource without cmd.exe redirection.  Keeping
# each path as a distinct CMake process argument avoids Windows quoting bugs
# when workspace paths contain spaces or drive-letter colons.
foreach(required PIPL_TOOL CXX_COMPILER AE_HEADERS PIPL_R PIPL_RR PIPL_RRC PIPL_RC)
  if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
    message(FATAL_ERROR "compile_pipl.cmake: required variable ${required} was not supplied")
  endif()
endforeach()

execute_process(
  COMMAND "${CXX_COMPILER}" "/I${AE_HEADERS}" "/I${AE_HEADERS}/SP" /EP "${PIPL_R}"
  OUTPUT_FILE "${PIPL_RR}"
  ERROR_VARIABLE preprocess_error
  RESULT_VARIABLE preprocess_result)
if(NOT preprocess_result EQUAL 0)
  message(FATAL_ERROR "PiPL first preprocessing pass failed (${preprocess_result}).\n${preprocess_error}")
endif()

execute_process(
  COMMAND "${PIPL_TOOL}" "${PIPL_RR}" "${PIPL_RRC}"
  RESULT_VARIABLE pipl_result
  OUTPUT_VARIABLE pipl_output
  ERROR_VARIABLE pipl_error)
if(NOT pipl_result EQUAL 0)
  message(FATAL_ERROR "PiPLtool failed (${pipl_result}).\n${pipl_output}\n${pipl_error}")
endif()

execute_process(
  COMMAND "${CXX_COMPILER}" /DMSWindows /EP "${PIPL_RRC}"
  OUTPUT_FILE "${PIPL_RC}"
  ERROR_VARIABLE resource_error
  RESULT_VARIABLE resource_result)
if(NOT resource_result EQUAL 0)
  message(FATAL_ERROR "PiPL resource preprocessing pass failed (${resource_result}).\n${resource_error}")
endif()

if(NOT EXISTS "${PIPL_RC}")
  message(FATAL_ERROR "PiPL resource output was not created: ${PIPL_RC}")
endif()
message(STATUS "Generated PiPL resource: ${PIPL_RC}")
