#include "code_parser.hpp"
#include <format>
#include <list>
#include "clang/cursor.hpp"
#include "clang/translate_unit.hpp"
#include "codegen/code_generator.hpp"
#include "types/type_context.hpp"

class MetaParser::Private {
public:

    void VisitAst (const Cursor &c);

    bool GenCode() const;

    void RecordTypes() const;

    std::string in_, out_;
    Namespace   current_ns_;
    std::list<TypeContext> contexts_;

    static std::vector<const char*> clang_args_;
    static std::list<std::string>   user_headers_;
    static std::list<std::string>   auto_register_contexts_;
};

std::vector<const char*> MetaParser::Private::clang_args_;
std::list<std::string>   MetaParser::Private::user_headers_;
std::list<std::string>   MetaParser::Private::auto_register_contexts_;

MetaParser::MetaParser (std::string input, std::string output): d{new Private} {
    d->in_ = std::move(input);
    d->out_ = std::move(output);
}

MetaParser::~MetaParser ()
{
    delete d;
}

bool MetaParser::Generate () const
{
    // 1. create translate unit
    TranslateUnit unit;
    if (!unit.Parse (d->in_.c_str (), &Private::clang_args_[0], (int)Private::clang_args_.size ()))
    {
        unit.ShowError();
        return false;
    }
    // 2. analysis ast, get all data we need
    const Cursor root = unit.GetCursor ();
    d->VisitAst(root);
    // 3. generate code
    if (!d->GenCode())
        return false;
    // 4. record type we need auto-register
    d->RecordTypes();
    return true;
}

void MetaParser::Prepare (std::vector<const char *> clang_args) {
    Private::clang_args_ = std::move(clang_args);
}

bool MetaParser::GenerateRegisterFile (const std::string &path)
{
    // generate function body
    AutoRegCodeGen gen(Private::user_headers_, Private::auto_register_contexts_, path);
    return gen.GenerateCode ();
}

void MetaParser::Private::VisitAst (const Cursor &c)
{
    auto children = c.Children (true);
    for (auto const &child : children)
    {
        if (child.IsNamespace ())
        {
            current_ns_.Push (child.Spelling ());
            VisitAst (child);
            current_ns_.Pop ();
        }
        else if (child.IsUserType () && !child.IsAnonymous ())
        {
            contexts_.emplace_back (child, current_ns_);
            if (!contexts_.back().ShouldCompile())
                contexts_.pop_back();
        }
    }
}

bool MetaParser::Private::GenCode () const
{
    TypeCodeGen gen(in_, contexts_, out_);
    return gen.GenerateCode ();
}

void MetaParser::Private::RecordTypes () const
{
    bool empty = true;
    for (auto const &ctx : contexts_)
    {
        if (ctx.ShouldCompile()) {
            auto_register_contexts_.emplace_back (ctx.GetName ());
            empty = false;
        }
    }
    if (!empty)
        user_headers_.emplace_back(in_);
}
