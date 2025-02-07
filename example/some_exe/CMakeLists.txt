project(some_exe CXX)

file(GLOB_RECURSE SRC_FILES *.*)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE libmeta some_lib)

meta_codegen(${PROJECT_NAME})
