#include <clang-c/Index.h>

#include "clang_utils.hpp"
#include "compile_options.hpp"
#include "header_pair.hpp"
#include "clang/cursor.hpp"

#include <iostream>
#include <string>

static void CreateFile(const char *path)
{
    FILE *f = fopen(path, "w");
    fclose(f);
}

static int ParseAndGen(const std::string &in, const std::string &out, const std::vector<const char*> &args)
{
    printf("Begin file: %s\n", in.c_str());
    int code = 0;
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit{};
    int parseFlags = 0;
    CXErrorCode ec = clang_parseTranslationUnit2(
        index, in.c_str(), &args[0], (int)args.size(),
        nullptr, 0, parseFlags, &unit);
    
    if (ec == CXError_Success) {
        Cursor cursor = clang_getTranslationUnitCursor(unit);

        auto children = cursor.Children(true);
        for (auto const &x: children) {
            if (x.IsUserType()) {
                Print(x);
                auto inner = x.Children(true);
                for (auto const &y: inner) {
                    Print(y);
                }
            }
        }
        clang_disposeTranslationUnit(unit);
    } else {
        code = 2;
        fprintf(stderr, "Failed to parse file \"%s\", code %d\n", in.c_str(), ec);
    }
    clang_disposeIndex(index);
    printf("End file: %s\n", in.c_str());
    return code;
}

/**
 * 需要传入的参数有：
 * 1. compile_headers.txt path
 * 2. auto_register.cpp path
 * 3. -I... -I
 * 4. -D... -D
 * 5. -H... -H
 */
int main(int argc, char const *argv[])
{
    if (argc != 6)
        return 1;
    
    // dump cmdl
    printf(R"(%s "%s" "%s" "%s" "%s" "%s")", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
    printf("\n");
    
    auto const args = ParseCompileOptions(argv + 3);
    auto const files = ParseHeaderPairFile(argv[1]);
    const std::string auto_register_file = argv[2];
    
    for (auto const &x: files) {
        ParseAndGen(x.first, x.second, args);
    }
    CreateFile(auto_register_file.c_str());


    return 0;
}
