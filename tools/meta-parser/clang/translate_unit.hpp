#ifndef TRANSLATE_UNIT_HPP
#define TRANSLATE_UNIT_HPP

#include <clang-c/Index.h>

class TranslateUnit final {
public:
    TranslateUnit();
    ~TranslateUnit();

    bool Parse(const char *file, const char **argv, int argc);

    void ShowError() const;

    [[nodiscard]] CXCursor GetCursor() const;

private:
    CXIndex           index_;
    CXTranslationUnit unit_;
};

#endif /* TRANSLATE_UNIT_HPP */
