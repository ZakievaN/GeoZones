// GeoZonesUnitTests.cpp: определ€ет точку входа дл€ консольного приложени€.
//

#include "stdafx.h"
#define CATCH_CONFIG_MAIN
#include "Catch.h"
#include "..\GeoZones\Sourse.h"

#pragma region --------------------ћетод вычислени€ улгов дл€ отрезков----------------------------------------

TEST_CASE("GetAngle90")
{
	REQUIRE(Sourse::GetAngle(5, 5, 5, 10) == 90.0f);
}

TEST_CASE("GetAngle180")
{
	REQUIRE(Sourse::GetAngle(5, 5, 0, 5 ) == 180.0f);
}

TEST_CASE("GetAngle270")
{
	REQUIRE(Sourse::GetAngle(5, 5, 5, 0) == 270.0f);
}

TEST_CASE("GetAngle0")
{
	REQUIRE(Sourse::GetAngle(5, 5, 10, 5) == 0.0f);
}

#pragma endregion

#pragma region --------------------ћетод определени€ точки пересечени€ двух отрезков----------------------------------------

void CheckIntersect(FLOATTYPE a1, FLOATTYPE a2, FLOATTYPE b1, FLOATTYPE b2, FLOATTYPE c1, FLOATTYPE c2, FLOATTYPE d1, FLOATTYPE d2, bool result)
{
	gpc_vertex a; a.x = a1; a.y = a2;
	gpc_vertex b; b.x = b1; b.y = b2;
	gpc_vertex c; c.x = c1; c.y = c2;
	gpc_vertex d; d.x = d1; d.y = d2;
	gpc_vertex * res = new gpc_vertex();
	FLOATTYPE r, s;
	REQUIRE(Sourse::Intersection(a, b, c, d, res, &r, &s) == result);
}

TEST_CASE("Intersect_not")
{
	CheckIntersect(0, 0, 4, 0, 5, 0, 10, 0, false);
}

TEST_CASE("Intersect_ab_cd")
{
	CheckIntersect(0, 0, 2, 2, 0, 2, 2, 0, true);
}

TEST_CASE("Intersect_acbd")
{
	CheckIntersect(0, 0, 6, 0, 4, 0, 10, 0, true);
}

TEST_CASE("Intersect_cadb")
{
	CheckIntersect(4, 0, 10, 0, 0, 0, 6, 0, true);
}

TEST_CASE("Intersect_adbc")
{
	CheckIntersect(0, 0, 6, 0, 10, 0, 4, 0, true);
}

TEST_CASE("Intersect_cbda")
{
	CheckIntersect(10, 0, 4, 0, 0, 0, 6, 0, true);
}

#pragma endregion
