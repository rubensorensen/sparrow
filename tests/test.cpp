#include "core.cpp"
#include "log.cpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test logging", "log") {
    REQUIRE(init_logging() == true);
}
