file(GLOB_RECURSE
  ALL_HEADERS
  *.*
)

add_library(libmeta_headers INTERFACE ${ALL_HEADERS})
target_include_directories(libmeta_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
source_tree(ALL_HEADERS)
set_property(TARGET libmeta_headers PROPERTY FOLDER "Library")
