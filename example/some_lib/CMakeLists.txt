project(some_lib CXX)

file(GLOB_RECURSE SRC_FILES *.*)

find_package(SDL3)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE libmeta SDL3::SDL3)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DLIB_EXPORT)

meta_codegen(${PROJECT_NAME})
