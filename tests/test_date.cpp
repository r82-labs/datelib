#define CATCH_CONFIG_MAIN
#include "datelib/date.h"

#include "catch2/catch.hpp"

TEST_CASE("hello_world returns greeting", "[hello]") {
    std::string result = datelib::hello_world();
    REQUIRE(result == "Hello, World!");
}
