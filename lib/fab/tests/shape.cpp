#include <catch/catch.hpp>

#include "fab/fab.h"
#include "fab/types/shape.h"
#include "fab/types/transform.h"

TEST_CASE("Transforming a shape")
{
    Shape a("+Xf1.0");

    // Apply a 2D transform to the shape
    Shape b = a.map(Transform("+Xf2.0","","",""));
    REQUIRE(b.math == "m+Xf2.0__+Xf1.0");
}

TEST_CASE("Transforming bounds")
{
    Bounds a(0, -2, 1, 2);

    // Apply a 2D (inverse) transform to the bounds
    Bounds b = a.map(Transform("","","+Xf2.0","*YY"));
    REQUIRE(b.xmin == 2);
    REQUIRE(b.xmax == 3);
    REQUIRE(b.ymin == -4);
    REQUIRE(b.ymax ==  4);
}

TEST_CASE("Invalid Shape construction")
{
    try {
        Shape a("+X");
        REQUIRE(false);
    } catch (fab::ParseError e) {
        REQUIRE(true);
    }
}
