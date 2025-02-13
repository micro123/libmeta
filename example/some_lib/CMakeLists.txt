project(some_lib CXX)

file(GLOB_RECURSE SRC_FILES *.*)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE libmeta)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DLIB_EXPORT)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# generate code for target
meta_codegen(${PROJECT_NAME})
