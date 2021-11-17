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

//0 - �� ������������, 1 - �������, 2 - ����������
enum TypeContour
{
	None = 0,
	General = 1,
	Closing = 2
};

//��� ����
enum TypeZone
{
	Closed = 1,
	NotClosed = 2
};

//����
class Zone
{
public:
	TypeZone typeZone;
	list<Line*> lines;
	Region * region;
	~Zone();
};

//����� ������
class LinesBorder
{
private:
	// ����������� �������� ��� �������
	FLOATTYPE CheckLine(gpc_vertex p1, gpc_vertex p2) const;	
public:
	LinesBorder* parent; // ��������, ���� hole == 1, ����� ������ ������
	bool isUse = false; // �������, ��� ������� ��� �������� ����-�� ���������
	TypeZone typeLines; // ��� ����� ������
	vector<Line*> lines; //�������
	vector<gpc_vertex> tops; //�������
	int hole = 0;
	FLOATTYPE GetArea(gpc_vertex a, gpc_vertex b, gpc_vertex c) const;
	~LinesBorder();
};

//�������
class Region
{
public:
	list<Zone*> zones;
	list<Line*> borders; // �������
	list<LinesBorder*> linesBorders; // �������
	bool isContained(Zone * zone);
	~Region();	
};

struct GZContour
{
	void sortPointIntersect();
	int typeCont = -1; //��� �������
	int num = -1; //�����
	TypeContour typeUseCont = None;
	gpc_vertex_list contour; //������
	GZContour();
	~GZContour();
	list<PointIntersect*> pointsIntersect; //����� ����������� � ��������
	bool ContainsPoint(PointIntersect * p);
};

class Line // ���������
{
public:
	Line* DublicateLine = nullptr;
	bool isDublicate = false; //���� ������ ��������
	FLOATTYPE AngleFrom = 0; //���� ������� ������� � ����� ����������� 
	FLOATTYPE AngleTo = 0; //���� ������� ������� � ����� �����������
	bool isDeleted = false; //������� ��� ������������ �������� ���������
	Zone * zone = nullptr; //������� ��� ��������������� ������� �� ���� 2
	LinesBorder * linesBorder = nullptr; // ������ �� ����� �������
	GZContour* contour = nullptr; //������
	int pointFromIndex = -1; //������ ����� �������
	int pointToIndex = -1; //��������� ����� �������
	PointIntersect* pointFromIntersect; //��������� ����� ���������
	PointIntersect* pointToInterserct; //�������� ����� ���������	
	Line(GZContour* icontour, PointIntersect* ipointFrom, PointIntersect* ipointTo, int from, int to, bool dublicate);
	void SetAngle(PointIntersect* original);	
};

struct IntersectInfo
{
	GZContour* contour; //������
	int segment = -1; // ����� ��������
	FLOATTYPE localArcCoordinate = -1; // ��������� ������� ����������
	PointIntersect * Parent;
};

class PointIntersect
{
private:
	bool Already�ontained(GZContour* contour, int segment, FLOATTYPE localArcCoordinate, gpc_vertex* point);
public:
	FLOATTYPE x, y;
	bool isDeleted = false; //������� ��� ������������ �������� ����� �����������
	bool isBad = false; //����� ����������� ������� ��� ����������� � ������ ������
	list<Line*> lines; //��������� ��� ����� �����������
	vector<IntersectInfo*> intersections; //�������� � �������� � �������������� ����� � �������
	~PointIntersect();
	void InitIntersect(GZContour* contour, GZContour* contour1, int segment, int segment1, FLOATTYPE localArcCoordinate, FLOATTYPE localArcCoordinate1, gpc_vertex* point);
	int GetSegmentOfContour(GZContour* contour, int ignoreSegment = -1); //����� �������� ��� ��������� ������� 		
	IntersectInfo* GetInfoForContour(GZContour* contour, int ignoreSegment = -1); //����� �������� ��� ��������� ������� 	
	bool ContainedLine(Line * line);
};

class GZContours
{
public:
	bool InvertXY = false;
	vector<GZContour*> contours; //��� �������
	vector<GZContour*> useMainContours; //������������ ������� �������
	vector<GZContour*> useClosedContours; //������������ ���������� �������
	vector<GZContour*> useContours; //��� ������������ �������
	map<int, vector<GZContour*>> contoursByType; //������� �� �����
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
	void GetInterserct(GZContour* currContour, GZContour* nextContour); //����� ����� ����������� ����� ����� ���������
	void GetAllPointsIntersect(); //����� ��� ����� �����������
	void AddLineToPointIntersect(PointIntersect* p, Line* line); // ���������� ��������� � ����� �����������
	void GetAllLines(); //����� ��� ���������
	void RemoveLineWithPointsIntersect(bool& isLeastOneSegmentDeleted, Line* a, PointIntersect* p1, PointIntersect* p2, PointIntersect* pointDeleted);
	void RemoveTails(); //������� ������ ��� ���� ��������
	void DeletePointIntersect(); //������� ��������� ����� �����������
	void DeleteLines(); //������� ��������� ���������
	void OverlappingLines(); //������������ �������� ��� �������� ����������� ����
	void SortLinesForPointIntersects(); //���������� �������� � ������ �����������
	void Step1();
	void Step2();
	Zone * GetZoneForLine(Line * line); // ����� ���� ������� ����������� �������� �������
	void Step3();
	void Step4();
	void Step5();
	void Step6();
	void Step7();
	bool isHole(LinesBorder * line, bool isUse);
	void Step8();
	gpc_polygon* Step9();
public:
	GZContours c; //�������� �������
	list<Zone*> simleZones; // ���� 
	list<Region*> regions; // �������	
	map<int, gpc_polygon*> zones; //����������� ����
	list<PointIntersect*> pointsIntersect; //����� ����������� ���� ��������
	list<Line*> lines; //��������� ���� ��������
	~GeoZonesObject();
	void GetRect(gpc_polygon*& zone);
	void GetZone(int typeZone, gpc_polygon* & zone);
	void GetZoneBase(int typeZone, gpc_polygon* & zone, bool secondEnter);
	void TransformCoordinatePoints(gpc_polygon* p); //����� ������� ��������� �����
	bool ContainsLine(Line * line);
};

#endif GEOZONES_H

