project(meta-parser C CXX)

file(GLOB_RECURSE SRC_FILES *.*)

find_package(Clang REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE libclang)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
