#include "code_generator.hpp"
#include <fstream>
#include <utilities/str_utils.hpp>
#include <utility>
#include "inja.hpp"
#include "refl/method.hpp"
#include "types/constant.hpp"
#include "types/field.hpp"
#include "types/function.hpp"
#include "types/type_context.hpp"
#include "types/user_type.hpp"

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
    using namespace Meta::Property; // property types are inside Meta::Property namespace

{# 4. type register bodies #}
## for type in ctx.types
// begin of {{type.fullname}}
    (void)TypeBuilder::NewTypeBuilder<{{type.fullname}}> ("{{type.name}}")
## for base in type.base_types
    .AddBaseType (GetTypeId<{{base}}>(), BaseCvt({{type.fullname}},{{base}}))
## endfor
## if type.has_properties
    .WithProperties({
## for prop in type.properties
        { "{{prop.name}}", {{prop.value}} },
## endfor
    })
## endif
## if type.add_default_ctor
    .AddDefaultConstructor<{{type.fullname}}>("{{type.name}}")
## endif
## for ctor in type.constructors
    .AddConstructor(
        MethodBuilder::NewMethodBuilder ("{{ctor.name}}", &Meta::Ctor::Of<{{type.fullname}}{{ctor.type}}>)
## for arg in ctor.args
        .AddParam ({{loop.index}}, "{{arg.name}}"{% if arg.has_init %}, (NParamT({{method.fullname}},{{loop.index}})){{arg.init}}{% endif %})
## endfor
        .Build()
    )
## if ctor.has_properties
    .WithProperties({
## for prop in ctor.properties
        { "{{prop.name}}", {{prop.value}} },
## endfor
    })
## endif
## endfor
## for field in type.fields
## if field.bitfield
    .AddField (BitField<{{type.fullname}}, decltype({{field.fullname}})>("{{field.name}}",  {{field.offset}}, {{field.width}}))
## else
    .AddField (MakeField ("{{field.name}}", &{{field.fullname}}))
## endif
## if field.has_properties
    .WithProperties({
## for prop in field.properties
        { "{{prop.name}}", {{prop.value}} },
## endfor
    })
## endif
## endfor
## for const in type.constants
    .AddConstant (MakeConstant ("{{const.name}}", {{const.value}}))
## if const.has_properties
    .WithProperties({
## for prop in const.properties
        { "{{prop.name}}", {{prop.value}} },
## endfor
    })
## endif
## endfor
## for method in type.methods
    .AddMethod(
        MethodBuilder::NewMethodBuilder ("{{method.name}}", Meta::Overloaded<{{method.type}}>::Of(&{{method.fullname}}))
## for arg in method.args
        .AddParam ({{loop.index}}, "{{arg.name}}"{% if arg.has_init %}, (NParamT({{method.fullname}},{{loop.index}})){{arg.init}}{% endif %})
## endfor
        .Build()
    )
## if method.has_properties
    .WithProperties({
## for prop in method.properties
        { "{{prop.name}}", {{prop.value}} },
## endfor
    })
## endif
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

static nlohmann::json BuildProperties(const TypeInfo *info)
{
    using namespace nlohmann;
    json arr = json::array();
    for (auto const &p: info->GetCustomProperties())
    {
        json prop = json::object();
        prop["name"] = p.name;
        prop["value"] = p.init;
        arr.emplace_back(std::move(prop));
    }
    return arr;
}

static nlohmann::json BuildConstant(const Constant *c)
{
    using namespace nlohmann;
    json j = json::object();
    j["name"] = toCamelCase(c->Name ());
    j["value"] = c->Value ();
    j["has_properties"] = c->HasProperties();
    j["properties"] = BuildProperties(c);
    return j;
}

static nlohmann::json BuildField(const Field *f)
{
    using namespace nlohmann;
    json j = json::object();
    j["name"] = toCamelCase(f->Name ());
    j["fullname"] = f->GetNamespace ().GetFullQualifiedName (f->Name ());
    j["has_properties"] = f->HasProperties();
    j["properties"] = BuildProperties(f);
    j["bitfield"] = f->IsBitField();
    j["offset"] = f->Offset();
    j["width"] = f->BitWidth();
    return j;
}

static nlohmann::json BuildConstructor(const Function *m)
{
    using namespace nlohmann;
    json j = json::object();
    j["name"] = m->Name();
    j["type"] = m->Type ();
    j["has_properties"] = m->HasProperties();
    j["properties"] = BuildProperties(m);
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

static nlohmann::json BuildMethod(const Function *m)
{
    using namespace nlohmann;
    json j = json::object();
    j["name"] = toCamelCase(m->Name ());
    j["fullname"] = m->GetNamespace ().GetFullQualifiedName (m->Name ());
    j["type"] = m->Type ();
    j["has_properties"] = m->HasProperties();
    j["properties"] = BuildProperties(m);
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
    t["add_default_ctor"] = !type.HasDefaultConstructorDefined();
    t["has_properties"] = type.HasProperties();
    t["properties"] = BuildProperties(&type);
    // base types
    json base_types = json::array();
    for (auto &x: type.BaseTypes())
    {
        base_types.emplace_back(x);
    }
    t["base_types"] = base_types;
    // constructors
    json constructors = json::array();
    for (auto x: type.Constructors())
    {
        constructors.emplace_back(BuildConstructor (x));
    }
    t["constructors"] = constructors;
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
        types.emplace_back(BuildType (*x));
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
