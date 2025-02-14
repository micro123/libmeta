project(some_exe CXX)

file(GLOB_RECURSE SRC_FILES *.*)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE libmeta some_lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
source_tree(SRC_FILES)

meta_codegen(${PROJECT_NAME})
