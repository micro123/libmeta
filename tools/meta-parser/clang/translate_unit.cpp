#include "translate_unit.hpp"
#include <iostream>
#include <ostream>
#include "utilities/clang_utils.hpp"

TranslateUnit::TranslateUnit ()
{
    index_ = clang_createIndex (0, 0);
    unit_  = nullptr;
}
TranslateUnit::~TranslateUnit ()
{
    if (unit_ != nullptr)
        clang_disposeTranslationUnit (unit_);
    clang_disposeIndex (index_);
}
bool TranslateUnit::Parse (const char *file, const char **argv, int argc)
{
    if (unit_ != nullptr)
        clang_disposeTranslationUnit (unit_);
    constexpr int     parseFlags = 0;
    const CXErrorCode ec         = clang_parseTranslationUnit2 (index_, file, argv, argc, nullptr, 0, parseFlags, &unit_);
    return ec == CXError_Success;
}
void TranslateUnit::ShowError () const
{
    if (unit_ != nullptr)
    {
        const auto set = clang_getDiagnosticSetFromTU (unit_);
        auto const cnt = clang_getNumDiagnosticsInSet (set);
        for (auto i = 0u; i < cnt; ++i)
        {
            const auto diagnostics = clang_getDiagnosticInSet (set, i);
            std::cout << clang_formatDiagnostic (diagnostics, 0x3F) << std::endl;
            clang_disposeDiagnostic (diagnostics);
        }
        clang_disposeDiagnosticSet (set);
    }
}

CXCursor TranslateUnit::GetCursor () const
{
    if (unit_ == nullptr)
        return clang_getNullCursor ();
    return clang_getTranslationUnitCursor (unit_);
}
