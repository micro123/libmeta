project(meta-parser C CXX)

file(GLOB_RECURSE SRC_FILES *.*)
source_tree(SRC_FILES)

find_package(Clang REQUIRED)

# Add path to LLVM modules
# set(CMAKE_MODULE_PATH
#   ${CMAKE_MODULE_PATH}
#   "${LLVM_CMAKE_DIR}"
# )

# import LLVM CMake functions
# include(AddLLVM)
find_library(CLANG_LIBRARY NAMES libclang clang PATHS ${LLVM_LIBRARY_DIRS} NO_DEFAULT_PATH NO_CACHE NO_SYSTEM_ENVIRONMENT_PATH REQUIRED)
message(STATUS "Clang library is ${CLANG_LIBRARY}")

# dump_cmake_variables(LLVM)
# dump_cmake_variables(CLANG)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${CLANG_LIBRARY} libmeta_headers)
target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${LLVM_INCLUDE_DIRS}
  ${CLANG_INCLUDE_DIRS}
)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools-bin")

if (WIN32)
  set(CLANG_DLL "${LLVM_TOOLS_BINARY_DIR}/libclang.dll")
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CLANG_DLL}"
      "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Copy libclang.dll to ${PROJECT_NAME}'s binary direcotry"
  )
endif ()

# message(STATUS "${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS} ${CLANG_LIBRARY}")

if (NOT WIN32)
  execute_process(COMMAND clang -v ERROR_VARIABLE CLANG_V)
  string(REGEX MATCH "Selected GCC installation: .*" CLANG_V ${CLANG_V})
  string(FIND ${CLANG_V} "\n" END_POS)
  string(SUBSTRING ${CLANG_V} 0 ${END_POS} CLANG_V)
  string(REPLACE "Selected GCC installation: " "" SYS_INCLUDE_PATH ${CLANG_V})
else ()
  set (SYS_INCLUDE_PATH "*")
endif ()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/system_include.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/system_include.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/system_include.cpp)
source_group("Generated Code" FILES ${CMAKE_CURRENT_BINARY_DIR}/system_include.cpp)

# 注册目标需要生成反射代码
function(meta_codegen tgt)
  # message(STATUS "Setting up target ${tgt} code gen")
  # 1. 获取代码生成目录
  get_target_property(BIN_DIR ${tgt} BINARY_DIR)
  set(CODE_GEN_DIR "${BIN_DIR}/_generated")
  # 2. 便利所有的源文件，准备生成代码
  get_target_property(SRC_DIR ${tgt} SOURCE_DIR)
  get_target_property(SRC_FILES ${tgt} SOURCES)
  set(ALL_HEADERS)
  foreach(file ${SRC_FILES})
    # 需要 .h/.hpp
    get_filename_component(suffix ${file} EXT)
    if (suffix MATCHES ".h|.hpp")
      list(APPEND ALL_HEADERS ${file})
    endif ()
  endforeach()
  message(STATUS "Target ${tgt} headers: ${ALL_HEADERS}")
  # 3. 判断是否需要生成代码
  if (ALL_HEADERS)
    # set(cmp_opts "${BIN_DIR}/compile_options.txt")
    set(cmp_hdrs "${BIN_DIR}/compile_headers.txt")
    set(reg_file "${CODE_GEN_DIR}/auto_register.cpp")

    # 3.2 compile_headers.txt
    set(GENERATED_FILES)
    file(WRITE ${cmp_hdrs} "")
    foreach(file ${ALL_HEADERS})
      get_filename_component(sdir ${file} DIRECTORY)
      get_filename_component(sname ${file} NAME_WE)
      file(RELATIVE_PATH rpath ${SRC_DIR} ${sdir})
      if (rpath)
        set(rpath "${rpath}/")
      endif()
      set(generated_file ${CODE_GEN_DIR}/${rpath}${sname}.meta.cpp)
      list(APPEND GENERATED_FILES ${generated_file})
      get_filename_component(generated_dir ${generated_file} DIRECTORY)
      file(MAKE_DIRECTORY ${generated_dir})
      
      # message (STATUS ">> CodeGen: ${file} -> ${generated_file}")
      # append line
      set(line "${file}|>${generated_file}\n")
      file(APPEND ${cmp_hdrs} ${line})
    endforeach()

    if (GENERATED_FILES)
      list(APPEND GENERATED_FILES ${reg_file})
      add_custom_command(
        OUTPUT ${GENERATED_FILES}
        COMMAND meta-parser
          ARGS
            ${cmp_hdrs} ${reg_file}
            "-I$<JOIN:$<TARGET_PROPERTY:${tgt},INCLUDE_DIRECTORIES>, -I>"
            "-D$<JOIN:$<TARGET_PROPERTY:${tgt},COMPILE_DEFINITIONS>, -D>"
            "-H$<JOIN:$<TARGET_PROPERTY:${tgt},PRECOMPILE_HEADERS>, -H>"
        COMMENT " >>> Executing mete-parser for ${tgt}"
        DEPENDS ${ALL_HEADERS} meta-parser
      )
      target_sources(${tgt} PRIVATE ${GENERATED_FILES})
      source_group(TREE ${CODE_GEN_DIR} PREFIX "Generated Files" FILES ${GENERATED_FILES})
      add_custom_target(
        ${tgt}_codegen
        COMMENT " >>> Code generation for target ${tgt}"
        DEPENDS ${GENERATED_FILES}
      )
      set_property(TARGET ${tgt}_codegen PROPERTY FOLDER "Generated Code")
      add_dependencies(
        ${tgt} ${tgt}_codegen
      )
    endif ()
  endif()
endfunction()
