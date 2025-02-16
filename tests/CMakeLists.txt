file(GLOB_RECURSE CATCH2_SRC catch2/*.*)
add_library(Catch2 STATIC ${CATCH2_SRC})
target_include_directories(Catch2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/catch2)
set_property(TARGET Catch2 PROPERTY FOLDER "Unit Tests/Lib")

file(GLOB_RECURSE TEST_COMMON_SRC test_types/*.*)
add_library(test_types OBJECT ${TEST_COMMON_SRC})
target_include_directories(test_types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/test_types)
set_property(TARGET test_types PROPERTY FOLDER "Unit Tests/Lib")
target_link_libraries(test_types PRIVATE libmeta libmeta_headers)
meta_codegen(test_types)

function(AddTest)
  set(options USE_COMMON)
  set(oneValueArgs NAME)
  set(multiValueArgs SRC LINK)
  cmake_parse_arguments(arg_add_test "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  set(name ${arg_add_test_NAME}_test)
  add_executable(${name} ${arg_add_test_SRC})
  target_link_libraries(${name} PRIVATE Catch2 ${arg_add_test_LINK})
  if (${arg_add_test_USE_COMMON})
    target_link_libraries(${name} PRIVATE test_types)
  endif ()
  add_test(NAME ${arg_add_test_NAME} COMMAND ${name})
  set_target_properties(
    ${name} PROPERTIES
    FOLDER "Unit Tests"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests-bin"
  )
  if (WIN32)
    add_custom_command(
      TARGET ${name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libmeta> $<TARGET_FILE_DIR:${name}>
      COMMENT "Copy runtime dlls to test binary directory"
    )
  endif()
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${arg_add_test_SRC})
endfunction()

macro(single_file_test name)
  if (NOT DEFINED ARGV1)
    AddTest(USE_COMMON OFF NAME ${name} SRC ${name}_test.cpp LINK libmeta libmeta_headers)
  else()
    AddTest(USE_COMMON ${ARGV1} NAME ${name} SRC ${name}_test.cpp LINK libmeta libmeta_headers)
  endif()
endmacro()

single_file_test(constexpr)
single_file_test(typeid)
single_file_test(registry)
single_file_test(ref)
single_file_test(field)
single_file_test(method)
single_file_test(any ON)
single_file_test(builder)
single_file_test(string_name)
single_file_test(serialize ON)
