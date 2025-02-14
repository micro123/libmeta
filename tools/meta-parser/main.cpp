#include <clang-c/Index.h>

#include "utilities/clang_utils.hpp"
#include "parser/compile_options.hpp"
#include "parser/header_pair.hpp"
#include "parser/code_parser.hpp"
#include "clang/cursor.hpp"

#include <iostream>
#include <string>
#include <list>

static bool ParseAndGen(const std::string &in, const std::string &out, const std::vector<const char*> &args)
{
    MetaParser parser(in, out);
    return parser.Generate();
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
    // printf(R"(%s "%s" "%s" "%s" "%s" "%s")", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
    // printf("\n");
    
    auto const args = ParseCompileOptions(argv + 3);
    auto const files = ParseHeaderPairFile(argv[1]);
    const std::string auto_register_file = argv[2];
    std::list<std::string> all_headers;
    
    MetaParser::Prepare(std::move(args));

    for (auto const &x: files) {
        if (!ParseAndGen(x.first, x.second, args))
        {
            return 1;
        }
        all_headers.emplace_back(x.first);
    }
    if (!MetaParser::GenerateRegisterFile(auto_register_file))
        return 1;

    return 0;
}
