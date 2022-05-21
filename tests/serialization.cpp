#include "parallelzone/serialization.hpp"
#include <catch2/catch.hpp>
#include <madness/world/buffer_archive.h>

TEMPLATE_TEST_CASE("Serialization with Cereal wrapper",
                   "[serialization][serializer][deserializer]",
                   cereal::BinaryOutputArchive,
                   parallelzone::BinaryOutputArchive,
                   parallelzone::PortableBinaryOutputArchive,
                   parallelzone::JSONOutputArchive,
                   parallelzone::XMLOutputArchive) {
    using output = TestType;
    using input  = typename parallelzone::get_input_from_output<output>::type;
    std::stringstream ss;
    REQUIRE(parallelzone::is_output_archive_v<output>);
    REQUIRE(parallelzone::is_input_archive_v<input>);
    SECTION("Plain-old-data") {
        {
            output ar(ss);
            ar << int{42} << double{3.14} << char{'R'};
        }

        int i;
        double d;
        char c;
        {
            input ar(ss);
            ar >> i >> d >> c;
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
            ar << v << m;
        }

        std::vector<int> v2;
        std::map<std::string, double> m2;
        {
            input ar(ss);
            ar >> v2 >> m2;
        }
        REQUIRE(v2 == v);
        REQUIRE(m2 == m);
    }
}

TEST_CASE("Serialization with MADNESS archive ") {
    unsigned char buf[32768];

    SECTION("Plain-old-data") {
        madness::archive::BufferOutputArchive oar(buf, sizeof(buf));
        oar << int{42} << double{3.14} << char{'K'};
        std::size_t nbyte = oar.size();
        oar.close();
        madness::archive::BufferInputArchive iar(buf, nbyte);
        int i;
        double d;
        char c;
        iar >> i >> d >> c;
        iar.close();
        REQUIRE(i == int{42});
        REQUIRE(d == double{3.14});
        REQUIRE_FALSE(d == float{3.14f});
        REQUIRE(c == char{'K'});
    }

    SECTION("Containers") {
        std::vector<int> v{1, 2, 3, 4};
        std::map<std::string, double> m{{"Hello", 1.23}, {"World", 3.14}};
        madness::archive::BufferOutputArchive oar(buf, sizeof(buf));
        oar << v << m;
        std::size_t nbyte = oar.size();
        oar.close();
        madness::archive::BufferInputArchive iar(buf, nbyte);
        std::vector<int> v2;
        std::map<std::string, double> m2;
        iar >> v2 >> m2;
        iar.close();
        REQUIRE(v2 == v);
        REQUIRE(m2 == m);
    }
}
