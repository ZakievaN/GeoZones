#ifndef GEOZONES_H
#define GEOZONES_H

#include <vector>
#include <map>
#include "gpc.h"
#include <list>

using namespace std;

class PointIntersect;
class Line;
class Region;

//0 - не используется, 1 - главный, 2 - замыкающий
enum TypeContour
{
	None = 0,
	General = 1,
	Closing = 2
};

//тип зоны
enum TypeZone
{
	Closed = 1,
	NotClosed = 2
};

//зоны
class Zone
{
public:
	TypeZone typeZone;
	list<Line*> lines;
	Region * region;
	~Zone();
};

//линии границ
class LinesBorder
{
private:
	// Контрольное значение для отрезка
	FLOATTYPE CheckLine(gpc_vertex p1, gpc_vertex p2) const;	
public:
	LinesBorder* parent; // родитель, если hole == 1, иначе пустая ссылка
	bool isUse = false; // признак, что граница уже является чьим-то родителем
	TypeZone typeLines; // тип линии границ
	vector<Line*> lines; //отрезки
	vector<gpc_vertex> tops; //вершины
	int hole = 0;
	FLOATTYPE GetArea(gpc_vertex a, gpc_vertex b, gpc_vertex c) const;
	~LinesBorder();
};

//области
class Region
{
public:
	list<Zone*> zones;
	list<Line*> borders; // границы
	list<LinesBorder*> linesBorders; // границы
	bool isContained(Zone * zone);
	~Region();	
};

struct GZContour
{
	void sortPointIntersect();
	int typeCont = -1; //тип контура
	int num = -1; //номер
	TypeContour typeUseCont = None;
	gpc_vertex_list contour; //контур
	GZContour();
	~GZContour();
	list<PointIntersect*> pointsIntersect; //точки пересечения с контуром
	bool ContainsPoint(PointIntersect * p);
};

class Line // отрезочек
{
public:
	Line* DublicateLine = nullptr;
	bool isDublicate = false; //если объект дубликат
	FLOATTYPE AngleFrom = 0; //угол наклона отрезка к точке пересечения 
	FLOATTYPE AngleTo = 0; //угол наклона отрезка к точке пересечения
	bool isDeleted = false; //пометка для последующего удаления отрезочка
	Zone * zone = nullptr; //пометка для использованного отрезка на шаге 2
	LinesBorder * linesBorder = nullptr; // ссылка на линию границы
	GZContour* contour = nullptr; //контур
	int pointFromIndex = -1; //первая точка контура
	int pointToIndex = -1; //последняя точка контура
	PointIntersect* pointFromIntersect; //начальная точка пересения
	PointIntersect* pointToInterserct; //конечная точка пересения	
	Line(GZContour* icontour, PointIntersect* ipointFrom, PointIntersect* ipointTo, int from, int to, bool dublicate);
	void SetAngle(PointIntersect* original);	
};

struct IntersectInfo
{
	GZContour* contour; //контур
	int segment = -1; // номер сегмента
	FLOATTYPE localArcCoordinate = -1; // локальная дуговая координата
	PointIntersect * Parent;
};

class PointIntersect
{
private:
	bool AlreadyСontained(GZContour* contour, int segment, FLOATTYPE localArcCoordinate, gpc_vertex* point);
public:
	FLOATTYPE x, y;
	bool isDeleted = false; //пометка для последующего удаления точки пересечения
	bool isBad = false; //точка пересечения удалена при объединении с другой точкой
	list<Line*> lines; //отрезочки для точки пересечения
	vector<IntersectInfo*> intersections; //сведения о контурах и местоположении точки в конутре
	~PointIntersect();
	void InitIntersect(GZContour* contour, GZContour* contour1, int segment, int segment1, FLOATTYPE localArcCoordinate, FLOATTYPE localArcCoordinate1, gpc_vertex* point);
	int GetSegmentOfContour(GZContour* contour, int ignoreSegment = -1); //взять сведения для заданного контура 		
	IntersectInfo* GetInfoForContour(GZContour* contour, int ignoreSegment = -1); //взять сведения для заданного контура 	
	bool ContainedLine(Line * line);
};

class GZContours
{
public:
	bool InvertXY = false;
	vector<GZContour*> contours; //все контуры
	vector<GZContour*> useMainContours; //используемые главные контуры
	vector<GZContour*> useClosedContours; //используемые замыкающие контуры
	vector<GZContour*> useContours; //все используемые контуры
	map<int, vector<GZContour*>> contoursByType; //контуры по типам
	~GZContours();
	void AddContour(int typeCont, int num, gpc_vertex_list* cont);
	void AddContour(int typeCont, int num, int nPoint, FLOATTYPE* x, FLOATTYPE* y);
	void InitContours();	
};

class GeoZonesObject
{
	void Clear();
	gpc_polygon* MakeZone();
	PointIntersect* FindPointIntersect(gpc_vertex* result);
	void GetInterserct(GZContour* currContour, GZContour* nextContour); //взять точки пересечения между двумя контурами
	void GetAllPointsIntersect(); //взять все точки пересечения
	void AddLineToPointIntersect(PointIntersect* p, Line* line); // добавление отрезочка к точке пересечения
	void GetAllLines(); //взять все отрезочки
	void RemoveLineWithPointsIntersect(bool& isLeastOneSegmentDeleted, Line* a, PointIntersect* p1, PointIntersect* p2, PointIntersect* pointDeleted);
	void RemoveTails(); //удалить хвосты для всех контуров
	void DeletePointIntersect(); //удалить помеченые точки пересечения
	void DeleteLines(); //удалить помеченые отрезочки
	void OverlappingLines(); //дублирование отрезков для контуров замыкающего типа
	void SortLinesForPointIntersects(); //сортировка отрезков в точках пересечения
	void Step1();
	void Step2();
	Zone * GetZoneForLine(Line * line); // взять зону которой принадлежит заданный отрезок
	void Step3();
	void Step4();
	void Step5();
	void Step6();
	void Step7();
	bool isHole(LinesBorder * line, bool isUse);
	void Step8();
	gpc_polygon* Step9();
public:
	GZContours c; //исходные контуры
	list<Zone*> simleZones; // зоны 
	list<Region*> regions; // области	
	map<int, gpc_polygon*> zones; //построенные зоны
	list<PointIntersect*> pointsIntersect; //точки пересечения всех контуров
	list<Line*> lines; //отрезочки всех контуров
	~GeoZonesObject();
	void GetRect(gpc_polygon*& zone);
	void GetZone(int typeZone, gpc_polygon* & zone);
	void GetZoneBase(int typeZone, gpc_polygon* & zone, bool secondEnter);
	void TransformCoordinatePoints(gpc_polygon* p); //смена местами координат точек
	bool ContainsLine(Line * line);
};

#endif GEOZONES_H

