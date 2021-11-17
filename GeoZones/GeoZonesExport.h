#ifndef GEOZONESEXPORT_H
#define GEOZONESEXPORT_H

#include "GeoZones.h"

#define GEOZONES_API extern "C" __declspec(dllexport)

//#pragma comment (linker, "/export:InitGZ=InitGZ@5")
GEOZONES_API void __stdcall InitGZ(void** pGeoZones, bool invetrXY);

//#pragma comment (linker, "/export:FreeGZ=FreeGZ@4") 
GEOZONES_API void __stdcall FreeGZ(void** pGeoZones);

//#pragma comment (linker, "/export:AddContour=AddContour@20") 
GEOZONES_API void __stdcall AddContour(void** pGeoZones, int typeCont, int num, int nPoint, gpc_vertex_list* cont);

//#pragma comment (linker, "/export:AddContourXY=AddContourXY@24") 
GEOZONES_API void __stdcall AddContourXY(void** pGeoZones, int typeCont, int num, int nPoint, float* x, float* y);

//#pragma comment (linker, "/export:AddContourXY=InitContours@4") 
GEOZONES_API void __stdcall InitContours(void** pGeoZones);

//#pragma comment (linker, "/export:GetZone=GetZone@12") 
GEOZONES_API void __stdcall GetZone(void** pGeoZones, int typeZone, gpc_polygon* &zone);

#endif GEOZONESEXPORT_H