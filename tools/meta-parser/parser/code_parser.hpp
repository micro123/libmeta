#ifndef CODE_PARSER_HPP
#define CODE_PARSER_HPP

#include <list>
#include <string>
#include <vector>

class MetaParser final {
public:
    MetaParser(std::string input, std::string output);
    ~MetaParser();

    bool Generate() const;

    static void Prepare(std::vector<const char*> clang_args);
    static bool GenerateRegisterFile(const std::string &path);
private:
    class Private;
    Private *const d;
};

#endif /* CODE_PARSER_HPP */
