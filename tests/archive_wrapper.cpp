#include "parallelzone/archive_wrapper.hpp"
#include <catch2/catch.hpp>

TEMPLATE_TEST_CASE("Serialization with  ArchiveWrapper",
                   "[serialization][Serializer][Deserializer]",
                   parallelzone::BinaryOutputArchive,
                   parallelzone::PortableBinaryOutputArchive,
                   parallelzone::JSONOutputArchive,
                   parallelzone::XMLOutputArchive) {
    using output = TestType;
    using input  = typename parallelzone::get_input_from_output<output>::type;

    std::stringstream ss;

    SECTION("Plain-old-data") {
        {
            output ar(ss);
            parallelzone::Serializer s(ar);
            s(int{42})(double{3.14})(char{'R'});
        }

        int i;
        double d;
        char c;

        {
            input ar(ss);
            parallelzone::Deserializer ds(ar);
            ds(i)(d)(c);
        }
        REQUIRE(i == int{42});
        REQUIRE(d == double{3.14});
        REQUIRE_FALSE(d == float{3.14f});
        REQUIRE(c == char{'R'});
    }

    SECTION("Containers") {
        std::vector<int> v{1, 2, 3, 4};
        std::map<std::string, double> m{{"Hello", 1.23}, {"World", 3.14}};

        {
            output ar(ss);
            parallelzone::Serializer s(ar);
            s(v)(m);
        }

        std::vector<int> v2;
        std::map<std::string, double> m2;
        {
            input ar(ss);
            parallelzone::Deserializer d(ar);
            d(v2)(m2);
        }
        REQUIRE(v2 == v);
        REQUIRE(m2 == m);
    }
}
