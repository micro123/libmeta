#include "compile_options.hpp"

#include <cstring>

extern const char *kSystemInclude;

std::vector<const char *> ParseCompileOptions (const char **first)
{
    // first[0]: includes
    // first[1]: definitions
    // first[2]: precompiled-headers

    std::vector<const char *> args {
        "-Xclang",
        "-ast-dump",
        "-xc++",
        "-std=c++20",
        "-DNDEBUG",
        "-D__clang__",
        "-D__META_CODEGEN__",
    }; // TODO: 添加默认参数

    // system headers
    if (kSystemInclude[2] != '*') {
        args.emplace_back(kSystemInclude);
    }

    char *includes = strdup(first[0]);
    char *defines = strdup(first[1]);
    char *headers = strdup(first[2]);

    // parse includes
    {
        int const len = strlen(includes);
        char *const end = includes + len;
        char *start = includes;
        char *pos = strstr(start+2, "-I"); // argv[3] always have atleast -I characters
        while(pos) {
            *(pos-1) = 0;
            args.emplace_back(start);
            start = pos;
            pos = strstr(start+2, "-I");
        }
        if (start + 2 < end) {
            args.emplace_back(start);
        }
    }

    // parse defines
    {
        int const len = strlen(defines);
        char *const end = defines + len;
        char *start = defines;
        char *pos = strstr(start+2, "-D"); // argv[4] always have atleast -D characters
        while(pos) {
            *(pos-1) = 0;
            args.emplace_back(start);
            start = pos;
            pos = strstr(start+2, "-D");
        }
        if (start + 2 < end) {
            args.emplace_back(start);
        }
    }

    // precompiled headers
    {
        int const len = strlen(headers);
        char *const end = headers + len;
        char *start = headers;
        char *pos = strstr(start+2, "-H"); // argv[5] always have atleast -H characters
        while(pos) {
            *(pos-1) = 0;
            args.emplace_back("-include");
            args.emplace_back(start+2);
            start = pos;
            pos = strstr(start+2, "-H");
        }
        if (start+2 < end) {
            args.emplace_back("-include");
            args.emplace_back(start+2);
        }
    }

    return args;
}
