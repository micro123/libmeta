set(CMAKE_FOLDER Library)

file(
    GLOB_RECURSE
    LIB_SOURCES
    *.*
)

add_library(
    ${PROJECT_NAME}
    SHARED
    ${LIB_SOURCES}
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    libmeta_headers
)

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
    -DLIBMETA_LIBRARY
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    OUTPUT_NAME "meta"
)

source_tree(LIB_SOURCES)
