#include <catch_amalgamated.hpp>
#include <utility/ref.hpp>

struct Foo {
    Foo(int) { buffer_ = malloc(100); }
    void Dispose() {
        free(buffer_);
        buffer_ = 0;
    }
    void *buffer_;
};

TEST_CASE("RefCounted Disposable") {
    auto f = Meta::MakeRef<Foo>(42);
    REQUIRE(f.RefCount() == 1);
}

TEST_CASE("RefCounted Simple") {
    Meta::Ref<int> i(new int{42});
    REQUIRE(i.RefCount() == 1);
    {
        auto j = i;
        REQUIRE(i.RefCount() == 2);
    }
    REQUIRE(i.RefCount() == 1);
}
