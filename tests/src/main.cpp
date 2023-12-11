#include <catch2/catch_test_macros.hpp>

static bool
run_a_function(void)
{
    return true;
}

TEST_CASE("Test testing", "test") {
    REQUIRE(run_a_function() == true);
}
