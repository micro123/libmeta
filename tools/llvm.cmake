if (TARGET llvm)
    return()
endif()

add_library(llvm SHARED IMPORTED GLOBAL)

option(LLVM_INSTALL_PATH "custom llvm install root path" "")

if (WIN32)
    # 默认安装路径 C:/Program Files/LLVM 和 C:/Program Files (x86)/LLVM
    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(LLVM_DEFAULT_DIR "C:/Program Files (x86)/LLVM")
    else()
        set(LLVM_DEFAULT_DIR "C:/Program Files/LLVM")
    endif()
    if (NOT ${LLVM_INSTALL_PATH} OR NOT EXISTS ${LLVM_INSTALL_PATH})
        set(LLVM_BASE ${LLVM_DEFAULT_DIR})
    else()
        set(LLVM_BASE ${LLVM_INSTALL_PATH})
    endif()
    set_target_properties(
        llvm PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${LLVM_BASE}/include
        IMPORTED_IMPLIB ${LLVM_BASE}/lib/libclang.lib
        IMPORTED_LOCATION ${LLVM_BASE}/bin/libclang.dll
    )
else()
    set_target_properties(
        llvm PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES /usr/include
        IMPORTED_LOCATION /usr/lib/libclang.so
    )
endif()
