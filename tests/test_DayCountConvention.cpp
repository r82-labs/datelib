#include "datelib/DayCountConvention.h"
#include "datelib/exceptions.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

using namespace std::chrono;
using namespace datelib;

// ============================================================================
// ActualActual Tests
// ============================================================================

TEST_CASE("ActualActual - same year calculation", "[DayCountConvention][ActualActual]") {
    ActualActual convention;

    SECTION("First half of 2024 (leap year)") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, July, day{1}};

        // 182 days (Jan 1 to Jul 1 in leap year)
        REQUIRE(convention.dayCount(start, end) == 182);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(182.0 / 366.0));
    }

    SECTION("First half of 2023 (non-leap year)") {
        const year_month_day start{year{2023}, January, day{1}};
        const year_month_day end{year{2023}, July, day{1}};

        // 181 days (Jan 1 to Jul 1 in non-leap year)
        REQUIRE(convention.dayCount(start, end) == 181);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(181.0 / 365.0));
    }

    SECTION("Quarter in leap year") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, April, day{1}};

        // 91 days
        REQUIRE(convention.dayCount(start, end) == 91);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(91.0 / 366.0));
    }
}

TEST_CASE("ActualActual - multi-year calculation", "[DayCountConvention][ActualActual]") {
    ActualActual convention;

    SECTION("Exactly one year in leap year") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2025}, January, day{1}};

        REQUIRE(convention.dayCount(start, end) == 366);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(1.0));
    }

    SECTION("Exactly one year in non-leap year") {
        const year_month_day start{year{2023}, January, day{1}};
        const year_month_day end{year{2024}, January, day{1}};

        REQUIRE(convention.dayCount(start, end) == 365);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(1.0));
    }

    SECTION("Two complete years") {
        const year_month_day start{year{2023}, January, day{1}};
        const year_month_day end{year{2025}, January, day{1}};

        REQUIRE(convention.dayCount(start, end) == 731); // 365 + 366
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(2.0));
    }

    SECTION("Partial year spanning year boundary") {
        const year_month_day start{year{2023}, July, day{1}};
        const year_month_day end{year{2024}, July, day{1}};

        // Should be approximately 1.0 year
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(1.0).epsilon(0.01));
    }
}

TEST_CASE("ActualActual - name and clone", "[DayCountConvention][ActualActual]") {
    ActualActual convention;

    SECTION("Name is correct") {
        REQUIRE(convention.name() == "Actual/Actual (ISDA)");
    }

    SECTION("Clone creates a copy") {
        auto clone = convention.clone();
        REQUIRE(clone != nullptr);
        REQUIRE(clone->name() == convention.name());

        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, July, day{1}};
        REQUIRE(clone->yearFraction(start, end) == convention.yearFraction(start, end));
    }
}

// ============================================================================
// Actual360 Tests
// ============================================================================

TEST_CASE("Actual360 - basic calculations", "[DayCountConvention][Actual360]") {
    Actual360 convention;

    SECTION("180 days") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, June, day{30}};

        REQUIRE(convention.dayCount(start, end) == 181);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(181.0 / 360.0));
    }

    SECTION("One year period") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2025}, January, day{1}};

        REQUIRE(convention.dayCount(start, end) == 366);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(366.0 / 360.0));
    }

    SECTION("30 days") {
        const year_month_day start{year{2024}, February, day{1}};
        const year_month_day end{year{2024}, March, day{2}};

        REQUIRE(convention.dayCount(start, end) == 30);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(30.0 / 360.0));
    }
}

TEST_CASE("Actual360 - name and clone", "[DayCountConvention][Actual360]") {
    Actual360 convention;

    SECTION("Name is correct") {
        REQUIRE(convention.name() == "Actual/360");
    }

    SECTION("Clone works correctly") {
        auto clone = convention.clone();
        REQUIRE(clone != nullptr);
        REQUIRE(clone->name() == "Actual/360");
    }
}

// ============================================================================
// Actual365Fixed Tests
// ============================================================================

TEST_CASE("Actual365Fixed - basic calculations", "[DayCountConvention][Actual365Fixed]") {
    Actual365Fixed convention;

    SECTION("Half year approximately") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, July, day{1}};

        REQUIRE(convention.dayCount(start, end) == 182);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(182.0 / 365.0));
    }

    SECTION("One year in leap year - still uses 365") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2025}, January, day{1}};

        REQUIRE(convention.dayCount(start, end) == 366);
        // Note: Actual/365 Fixed always uses 365, even in leap years
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(366.0 / 365.0));
    }

    SECTION("90 days") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, March, day{31}};

        REQUIRE(convention.dayCount(start, end) == 90);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(90.0 / 365.0));
    }
}

TEST_CASE("Actual365Fixed - name and clone", "[DayCountConvention][Actual365Fixed]") {
    Actual365Fixed convention;

    SECTION("Name is correct") {
        REQUIRE(convention.name() == "Actual/365 (Fixed)");
    }

    SECTION("Clone works correctly") {
        auto clone = convention.clone();
        REQUIRE(clone != nullptr);
        REQUIRE(clone->name() == "Actual/365 (Fixed)");
    }
}

// ============================================================================
// Thirty360 Tests
// ============================================================================

TEST_CASE("Thirty360 - basic calculations", "[DayCountConvention][Thirty360]") {
    Thirty360 convention;

    SECTION("One month (30 days assumed)") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, February, day{1}};

        REQUIRE(convention.dayCount(start, end) == 30);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(30.0 / 360.0));
    }

    SECTION("One year (360 days)") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2025}, January, day{1}};

        REQUIRE(convention.dayCount(start, end) == 360);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(1.0));
    }

    SECTION("Six months") {
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, July, day{1}};

        REQUIRE(convention.dayCount(start, end) == 180);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(0.5));
    }
}

TEST_CASE("Thirty360 - 31st day adjustments", "[DayCountConvention][Thirty360]") {
    Thirty360 convention;

    SECTION("Start on 31st - adjusted to 30") {
        const year_month_day start{year{2024}, January, day{31}};
        const year_month_day end{year{2024}, February, day{1}};

        // Jan 31 -> 30, Feb 1 -> 1, so (1 - 30) + (2-1)*30 = -29 + 30 = 1
        REQUIRE(convention.dayCount(start, end) == 1);
    }

    SECTION("End on 31st when start is 30 or 31") {
        const year_month_day start{year{2024}, January, day{31}};
        const year_month_day end{year{2024}, March, day{31}};

        // Jan 31 -> 30, Mar 31 -> 30 (because start is 31)
        // (30 - 30) + (3-1)*30 = 0 + 60 = 60
        REQUIRE(convention.dayCount(start, end) == 60);
    }

    SECTION("End on 31st when start is not 30 or 31") {
        const year_month_day start{year{2024}, January, day{15}};
        const year_month_day end{year{2024}, March, day{31}};

        // Jan 15 stays 15, Mar 31 stays 31 (because start is 15, not 30/31)
        // (31 - 15) + (3-1)*30 = 16 + 60 = 76
        REQUIRE(convention.dayCount(start, end) == 76);
    }
}

TEST_CASE("Thirty360 - February handling", "[DayCountConvention][Thirty360]") {
    Thirty360 convention;

    SECTION("End of February in non-leap year") {
        const year_month_day start{year{2023}, February, day{1}};
        const year_month_day end{year{2023}, February, day{28}};

        // No special adjustments, just regular calculation
        REQUIRE(convention.dayCount(start, end) == 27);
    }

    SECTION("End of February in leap year") {
        const year_month_day start{year{2024}, February, day{1}};
        const year_month_day end{year{2024}, February, day{29}};

        REQUIRE(convention.dayCount(start, end) == 28);
    }
}

TEST_CASE("Thirty360 - name and clone", "[DayCountConvention][Thirty360]") {
    Thirty360 convention;

    SECTION("Name is correct") {
        REQUIRE(convention.name() == "30/360 (Bond Basis)");
    }

    SECTION("Clone works correctly") {
        auto clone = convention.clone();
        REQUIRE(clone != nullptr);
        REQUIRE(clone->name() == "30/360 (Bond Basis)");
    }
}

// ============================================================================
// Polymorphism and Edge Cases
// ============================================================================

TEST_CASE("DayCountConvention - polymorphic usage", "[DayCountConvention]") {
    SECTION("Can use through base class pointer") {
        std::unique_ptr<DayCountConvention> convention = std::make_unique<ActualActual>();

        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, July, day{1}};

        REQUIRE(convention->name() == "Actual/Actual (ISDA)");
        REQUIRE(convention->dayCount(start, end) == 182);
        REQUIRE(convention->yearFraction(start, end) == Catch::Approx(182.0 / 366.0));
    }

    SECTION("Clone through base class pointer") {
        std::unique_ptr<DayCountConvention> original = std::make_unique<Actual360>();
        auto clone = original->clone();

        REQUIRE(clone->name() == original->name());
    }
}

TEST_CASE("DayCountConvention - same date", "[DayCountConvention]") {
    const year_month_day date{year{2024}, March, day{15}};

    SECTION("ActualActual - zero fraction") {
        ActualActual convention;
        REQUIRE(convention.dayCount(date, date) == 0);
        REQUIRE(convention.yearFraction(date, date) == 0.0);
    }

    SECTION("Actual360 - zero fraction") {
        Actual360 convention;
        REQUIRE(convention.dayCount(date, date) == 0);
        REQUIRE(convention.yearFraction(date, date) == 0.0);
    }

    SECTION("Actual365Fixed - zero fraction") {
        Actual365Fixed convention;
        REQUIRE(convention.dayCount(date, date) == 0);
        REQUIRE(convention.yearFraction(date, date) == 0.0);
    }

    SECTION("Thirty360 - zero fraction") {
        Thirty360 convention;
        REQUIRE(convention.dayCount(date, date) == 0);
        REQUIRE(convention.yearFraction(date, date) == 0.0);
    }
}

TEST_CASE("DayCountConvention - invalid date handling", "[DayCountConvention]") {
    ActualActual convention;

    SECTION("Invalid start date throws") {
        const year_month_day invalid{year{2024}, February, day{30}}; // Invalid
        const year_month_day valid{year{2024}, March, day{1}};

        REQUIRE_THROWS_AS(convention.yearFraction(invalid, valid), InvalidDateException);
    }

    SECTION("Invalid end date throws") {
        const year_month_day valid{year{2024}, March, day{1}};
        const year_month_day invalid{year{2024}, February, day{30}}; // Invalid

        REQUIRE_THROWS_AS(convention.yearFraction(valid, invalid), InvalidDateException);
    }

    SECTION("Start after end throws") {
        const year_month_day start{year{2024}, July, day{1}};
        const year_month_day end{year{2024}, January, day{1}};

        REQUIRE_THROWS_AS(convention.yearFraction(start, end), std::invalid_argument);
    }
}

TEST_CASE("DayCountConvention - comparison between conventions",
          "[DayCountConvention][comparison]") {
    const year_month_day start{year{2024}, January, day{1}};
    const year_month_day end{year{2024}, July, day{1}};

    ActualActual aa;
    Actual360 a360;
    Actual365Fixed a365;
    Thirty360 t360;

    // All should agree on the actual day count (except 30/360)
    const int actual_days = aa.dayCount(start, end);
    REQUIRE(a360.dayCount(start, end) == actual_days);
    REQUIRE(a365.dayCount(start, end) == actual_days);
    // 30/360 will differ due to its convention

    // Year fractions should differ based on denominator
    const double aa_fraction = aa.yearFraction(start, end);
    const double a360_fraction = a360.yearFraction(start, end);
    const double a365_fraction = a365.yearFraction(start, end);

    // Actual/360 should give larger fraction than Actual/365
    REQUIRE(a360_fraction > a365_fraction);

    // 30/360 should give exactly 0.5 for 6 months
    REQUIRE(t360.yearFraction(start, end) == Catch::Approx(0.5));
}

TEST_CASE("DayCountConvention - real-world examples", "[DayCountConvention][examples]") {
    SECTION("3-month US Treasury bill (Actual/360)") {
        Actual360 convention;
        const year_month_day start{year{2024}, March, day{1}};
        const year_month_day end{year{2024}, June, day{1}};

        // 92 actual days
        const double fraction = convention.yearFraction(start, end);
        REQUIRE(fraction == Catch::Approx(92.0 / 360.0));
    }

    SECTION("6-month corporate bond (30/360)") {
        Thirty360 convention;
        const year_month_day start{year{2024}, January, day{15}};
        const year_month_day end{year{2024}, July, day{15}};

        // Exactly 6 months = 180 days in 30/360
        const double fraction = convention.yearFraction(start, end);
        REQUIRE(fraction == Catch::Approx(0.5));
    }

    SECTION("Annual interest calculation (Actual/Actual)") {
        ActualActual convention;
        const year_month_day start{year{2024}, January, day{1}};
        const year_month_day end{year{2024}, December, day{31}};

        // 365 days in leap year 2024
        const int days = convention.dayCount(start, end);
        REQUIRE(days == 365);
        REQUIRE(convention.yearFraction(start, end) == Catch::Approx(365.0 / 366.0));
    }
}
