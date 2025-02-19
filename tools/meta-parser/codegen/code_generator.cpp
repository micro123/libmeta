#include "code_generator.hpp"
#include <fstream>
#include <utility>
#include "inja.hpp"
#include "types/constant.hpp"
#include "types/field.hpp"
#include "types/function.hpp"
#include "types/type_context.hpp"
#include "types/user_type.hpp"
#include <utilities/str_utils.hpp>

class TypeCodeGen::Private
{
public:
    const std::list<TypeContext> *contexts_;
    std::string header_;
    std::string out_path_;
};

TypeCodeGen::TypeCodeGen (std::string header, const std::list<TypeContext> &types, std::string output): d(new Private)
{
    d->contexts_ = &types;
    d->header_ = std::move(header);
    d->out_path_ = std::move(output);
}
TypeCodeGen::~TypeCodeGen () { delete d; }

constexpr auto type_reg_template = R"(
{# 1. fixed headers from libmeta #}
#include <refl/builders.hpp>
#include <refl/field.hpp>
#include <refl/constant.hpp>

{# 2. user headers #}
#include "{{header}}"

{# 3. generated body #}
## for ctx in contexts
template<>
void Meta::CodeGenFor<{{ctx.name}}>::Register() {

{# 4. type register bodies #}
## for type in ctx.types
// begin of {{type.fullname}}
    (void)TypeBuilder::NewTypeBuilder<{{type.fullname}}> ("{{type.name}}")
## for field in type.fields
    .AddField (MakeField ("{{field.name}}", &{{field.fullname}}))
## endfor
## for const in type.constants
    .AddConstant (MakeConstant ("{{const.name}}", {{const.value}}))
## endfor
## for method in type.methods
    .AddMethod(
        MethodBuilder::NewMethodBuilder ("{{method.name}}", &{{method.fullname}})
## for arg in method.args
        .AddParam ({{loop.index}}, "{{arg.name}}"{% if arg.has_init %}, (NParamT({{method.fullname}},{{loop.index}})){{arg.init}}{% endif %})
## endfor
        .Build()
    )
## endfor
## if type.is_enum
    .AddEnumOperations<{{type.fullname}}>()
## endif
    ;
// end of {{type.fullname}}


## endfor
};

## endfor
)";

static nlohmann::json BuildConstant(const Constant *c)
{
    using namespace nlohmann;
    json j = json::object();
    j["name"] = toCamelCase(c->Name ());
    j["value"] = c->Value ();
    return j;
}

static nlohmann::json BuildField(const Field *f)
{
    using namespace nlohmann;
    json j = json::object();
    j["name"] = toCamelCase(f->Name ());
    j["fullname"] = f->GetNamespace ().GetFullQualifiedName (f->Name ());
    return j;
}

static nlohmann::json BuildMethod(const Function *m)
{
    using namespace nlohmann;
    json j = json::object();
    j["name"] = toCamelCase(m->Name ());
    j["fullname"] = m->GetNamespace ().GetFullQualifiedName (m->Name ());
    json args = json::array();
    for (auto &x: m->Arguments())
    {
        json arg = json::object();
        arg["name"] = x.name;
        arg["has_init"] = !x.init_value.empty();
        arg["init"] = x.init_value;
        args.emplace_back(arg);
    }
    j["args"] = args;
    return j;
}

static nlohmann::json BuildType(const LanguageType &type)
{
    using namespace nlohmann;
    json t = json::object();
    t["fullname"] = type.FullName();
    t["name"] = type.Name();
    t["is_enum"] = type.IsEnum();
    // constants
    json constants = json::array();
    for (auto &x: type.Constants ())
    {
        constants.emplace_back(BuildConstant (x));
    }
    t["constants"] = constants;
    // fields
    json fields = json::array();
    for (auto &x: type.Fields ())
    {
        fields.emplace_back(BuildField (x));
    }
    t["fields"] = fields;
    // methods
    json methods = json::array();
    for (auto &x: type.Functions ())
    {
        methods.emplace_back(BuildMethod (x));
    }
    t["methods"] = methods;
    return t;
}

static nlohmann::json BuildContext (const TypeContext &tc)
{
    using namespace nlohmann;
    json ctx = json::object();
    ctx["name"] = tc.GetName ();
    json types = json::array();
    for (auto &x: tc.GetTypes ())
    {
        types.emplace_back(BuildType (x));
    }
    ctx["types"] = types;
    return ctx;
}

bool TypeCodeGen::GenerateCode () const
{
    using namespace inja;
    using namespace nlohmann;

    std::ofstream out(d->out_path_, std::ios::out | std::ios::trunc);

    if (!d->contexts_->empty()) {
        json data = json::object();
        data["header"] = d->header_;
        json contexts = json::array();
        for (const auto &ctx : *d->contexts_)
        {
            contexts.emplace_back(BuildContext (ctx));
        }
        data["contexts"] = contexts;
        render_to (out, type_reg_template, data);
    }

    out.close();
    return true;
}


class AutoRegCodeGen::Private
{
public:
    std::list<std::string> headers_;
    std::list<std::string> contexts_;
    std::string out_path_;
};

AutoRegCodeGen::AutoRegCodeGen (const std::list<std::string> &headers, const std::list<std::string> &types, std::string output): d{new Private}
{
    d->headers_ = headers;
    d->contexts_ = types;
    d->out_path_ = std::move(output);
}
AutoRegCodeGen::~AutoRegCodeGen () { delete d; }

constexpr auto auto_reg_template = R"(
{# 1. fixed headers #}
#include <meta-parser.h>

{# 2. user sources #}
## for header in headers
#include "{{header}}"
## endfor

{# 3. extern define #}
## for type in types
extern template struct Meta::CodeGenFor<{{type}}>;
## endfor

{# 4. function start #}
void RegisterTypes() {

{# 5. call register functions #}
## for type in types
    Meta::CodeGenFor<{{type}}>::Register();
## endfor


}

{# 6. auto call #}
static struct AutoRegister_ { AutoRegister_(){ RegisterTypes(); } } auto_register;

{# 7. end #}
)";

bool AutoRegCodeGen::GenerateCode () const
{
    using namespace inja;
    using namespace nlohmann;
    std::ofstream out(d->out_path_, std::ios::out | std::ios::trunc);

    if (!d->headers_.empty() && !d->contexts_.empty())
    {
        // build json
        json data = json::object();
        // headers
        json headers = json::array();
        for (const auto &header : d->headers_)
            headers.push_back(header);
        data["headers"] = headers;
        // types
        json types = json::array();
        for (const auto &type : d->contexts_)
            types.push_back(type);
        data["types"] = types;

        render_to (out, auto_reg_template, data);
    }
    out.close();
    return true;
}
