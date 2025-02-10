#include <catch2/catch_test_macros.hpp>
#include <pasio/pasio/core/core.hpp>

namespace pasio::testing {
    inline constexpr auto tags = "[pasio]";

    TEST_CASE("Dummy test for test resolve", tags) {
        CHECK(true);
    }
}
