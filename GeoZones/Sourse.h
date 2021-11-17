#pragma once
#include "GeoZones.h"
#include "gpc.h"

#pragma once
static class __declspec(dllexport) Sourse
{
public:

	Sourse()
	{
	}

	virtual ~Sourse()
	{
	}

	static bool Intersection(gpc_vertex a, gpc_vertex b, gpc_vertex c, gpc_vertex d, gpc_vertex * p, FLOATTYPE * r, FLOATTYPE * s);

	static FLOATTYPE GetAngle(FLOATTYPE x, FLOATTYPE y, FLOATTYPE x1, FLOATTYPE y1);

	static gpc_polygon* Sourse::newPolygon();

	static void Sourse::freePolygon(gpc_polygon*& p);

	static bool Sourse::IsInside(vector<gpc_vertex> tops, float x, float y);

};

