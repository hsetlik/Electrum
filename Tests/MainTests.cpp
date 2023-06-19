#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <PluginEditor.h>

TEST_CASE("example test", "[examples]")
{
    REQUIRE(true);
}

TEST_CASE("perpincicular distance", "[mathutil]")
{
    float value = Math::perpindicularDistance(0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);
    REQUIRE(value == (std::sqrt(2.0f) / 2.0f));
    value = Math::perpindicularDistance(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    REQUIRE(value == 0.0f);
    value = Math::perpindicularDistance(1.0f, 0.0f, 2.0f, 1.0f, 2.0f, 0.0f);
    REQUIRE(value == (std::sqrt(2.0f) / 2.0f));
}
