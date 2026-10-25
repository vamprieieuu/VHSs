# compile_pipl.cmake - Preprocess and compile Adobe PiPL .r file to .rc
if(NOT DEFINED PIPL_TOOL OR NOT DEFINED PIPL_R OR NOT DEFINED PIPL_RC)
  message(FATAL_ERROR "compile_pipl.cmake requires PIPL_TOOL, PIPL_R, and PIPL_RC")
endif()

get_filename_component(PIPL_DIR "${PIPL_RC}" DIRECTORY)
file(MAKE_DIRECTORY "${PIPL_DIR}")

# Step 1: Preprocess .r to .rrc with cl.exe or fallback
execute_process(
  COMMAND "${CXX_COMPILER}" /nologo /EP /TC /I "${AE_HEADERS}" /I "${AE_HEADERS}/SP" "${PIPL_R}"
  OUTPUT_FILE "${PIPL_RRC}"
  RESULT_VARIABLE PREPROCESS_RES
)

if(NOT PREPROCESS_RES EQUAL 0)
  message(WARNING "Preprocess step exited with ${PREPROCESS_RES}. Attempting direct PiPLtool execution...")
  set(PIPL_INPUT "${PIPL_R}")
else()
  set(PIPL_INPUT "${PIPL_RRC}")
endif()

# Step 2: Compile with PiPLtool.exe
execute_process(
  COMMAND "${PIPL_TOOL}" "${PIPL_INPUT}" "${PIPL_RC}"
  RESULT_VARIABLE PIPL_RES
)

if(NOT PIPL_RES EQUAL 0)
  message(FATAL_ERROR "PiPLtool failed with error ${PIPL_RES}")
endif()

message(STATUS "Generated PiPL resource: ${PIPL_RC}")
