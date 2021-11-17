#include "GeoZonesExport.h"

void __stdcall InitGZ(void** pGeoZones, bool invetrXY)
{
	GeoZonesObject* gzObj = new GeoZonesObject;
	gzObj->c.InvertXY = invetrXY;
	*pGeoZones = static_cast<void*>(gzObj);
}

void __stdcall AddContour(void** pGeoZones, int typeCont, int num, gpc_vertex_list* cont)
{
	GeoZonesObject* gzObj = static_cast<GeoZonesObject*>(*pGeoZones);
	gzObj->c.AddContour(typeCont, num, cont);
}

void __stdcall AddContourXY(void** pGeoZones, int typeCont, int num, int nPoint, float* x, float* y)
{
	GeoZonesObject* gzObj = static_cast<GeoZonesObject*>(*pGeoZones);
	gzObj->c.AddContour(typeCont, num, nPoint, x, y);
}

void __stdcall InitContours(void** pGeoZones)
{
	GeoZonesObject* gzObj = static_cast<GeoZonesObject*>(*pGeoZones);
	gzObj->c.InitContours();
}

void __stdcall FreeGZ(void** pGeoZones)
{
	if (pGeoZones != nullptr)
	{
		GeoZonesObject* gzObj = static_cast<GeoZonesObject*>(*pGeoZones);
		delete gzObj;
		*pGeoZones = nullptr;
	}
}

void __stdcall GetZone(void** pGeoZones, int typeZone, gpc_polygon* &zone)
{
	GeoZonesObject* gzObj = static_cast<GeoZonesObject*>(*pGeoZones);
	gzObj->GetZone(typeZone, zone);
}