#include <catch_amalgamated.hpp>
#include <utility/ref.hpp>

struct Foo {
    Foo (int)
    {
        buffer_ = malloc (100);
        puts ("Foo");
    }
    void Dispose ()
    {
        puts ("Dispose");
        free (buffer_);
        buffer_ = 0;
    }
    void Bar ()
    {
        memset (buffer_, 0x42, 100);
    }
    void *buffer_;
};

TEST_CASE ("RefCounted Disposable")
{
    auto f = Meta::MakeRef<Foo> (42);
    REQUIRE (f.RefCount () == 1);
    {
        auto x = std::move (f);
        REQUIRE (f == nullptr);
        x->Bar ();
    }
}

TEST_CASE ("RefCounted Simple")
{
    Meta::Ref<int> i (new int {42});
    REQUIRE (i.RefCount () == 1);
    {
        auto j = i;
        REQUIRE (i.RefCount () == 2);
    }
    REQUIRE (i.RefCount () == 1);
}
