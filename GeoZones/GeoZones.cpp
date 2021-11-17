#include "Sourse.h"
#include "GeoZones.h"
#include <vector>
#include <algorithm>
#include <set>
#include "RamerDouglasPeucker.h"

#pragma region --------------------PointIntersect----------------------------------------

PointIntersect::~PointIntersect()
{
	for (auto it = intersections.begin(); it != intersections.end(); ++it)
		delete *it;
	intersections.clear();
	lines.clear();
}

bool PointIntersect::AlreadyСontained(GZContour* contour, int segment, FLOATTYPE localArcCoordinate, gpc_vertex* point)
{
	for (auto info = intersections.begin(); info != intersections.end(); ++info)
	{
		if ((*info)->contour == contour && (*info)->segment == segment && (*info)->localArcCoordinate == localArcCoordinate)
		{
			return true;
		}
	}
	return false;
}

void PointIntersect::InitIntersect(GZContour* contour, GZContour* contour1, int segment, int segment1, FLOATTYPE localArcCoordinate, FLOATTYPE localArcCoordinate1, gpc_vertex* point)
{
	if (!AlreadyСontained(contour, segment, localArcCoordinate, point))
	{
		IntersectInfo* info = new IntersectInfo();
		(*info).contour = contour;
		(*info).segment = segment;
		(*info).localArcCoordinate = localArcCoordinate;
		(*info).Parent = this;
		intersections.push_back(info);
	}

	if (!AlreadyСontained(contour1, segment1, localArcCoordinate1, point))
	{
		IntersectInfo* info1 = new IntersectInfo();
		(*info1).contour = contour1;
		(*info1).segment = segment1;
		(*info1).localArcCoordinate = localArcCoordinate1;
		(*info1).Parent = this;
		intersections.push_back(info1);
	}
	x = (*point).x;
	y = (*point).y;
}

int PointIntersect::GetSegmentOfContour(GZContour* c, int ignoreSegment)
{
	for (auto info = intersections.begin(); info != intersections.end(); ++info)
	{
		auto& i = *info;
		if ((*i).contour == c)
		{
			if (ignoreSegment != i->segment)
			{
				return i->segment;
			}
		}
	}
	return -1;
}

IntersectInfo* PointIntersect::GetInfoForContour(GZContour* c, int ignoreSegment)
{
	for (auto info = intersections.begin(); info != intersections.end(); ++info)
	{
		auto& i = *info;
		if ((*i).contour == c)
		{
			if (ignoreSegment != i->segment)
			{
				return i;
			}
		}
	}
	return nullptr;
}

bool PointIntersect::ContainedLine(Line* line)
{
	for (auto l = lines.begin(); l != lines.end(); ++l)
	{
		if (*l == line)
		{
			return true;
		}
	}
	return false;
}
#pragma endregion

#pragma region --------------------Line--------------------------------------------------

/// <summary> Формирование отрезка </summary>  
Line::Line(GZContour* icontour, PointIntersect* ipointFrom, PointIntersect* ipointTo, int from, int to, bool idublicate)
{
	contour = icontour;
	pointFromIntersect = idublicate ? ipointTo : ipointFrom;
	pointToInterserct = idublicate ? ipointFrom : ipointTo;
	pointFromIndex = idublicate ? to : from;
	pointToIndex = idublicate ? from : to;
	isDublicate = idublicate;
}

void Line::SetAngle(PointIntersect* original)
{
	if (isDublicate) return;
	FLOATTYPE x, y;
	int i;

	if (original == pointFromIntersect && original == pointToInterserct)//петля
	{
		i = pointFromIndex != -1 ? pointFromIndex : pointFromIntersect->GetSegmentOfContour(contour);
		x = contour->contour.vertex[i].x;
		y = contour->contour.vertex[i].y;
		AngleFrom = Sourse::GetAngle(original->x, original->y, x, y);
		i = pointToIndex != -1 ? pointToIndex : pointToInterserct->GetSegmentOfContour(contour);
		x = contour->contour.vertex[i].x;
		y = contour->contour.vertex[i].y;
		AngleTo = Sourse::GetAngle(original->x, original->y, x, y);
	}
	else
	{
		if (original == pointFromIntersect && AngleFrom == 0)
		{
			i = pointFromIndex != -1 ? pointFromIndex : pointFromIntersect->GetSegmentOfContour(contour)+1;
			x = contour->contour.vertex[i].x;
			y = contour->contour.vertex[i].y;
			AngleFrom = Sourse::GetAngle(original->x, original->y, x, y);
		} 
		else if (original == pointToInterserct && AngleTo == 0)
		{
			i = pointToIndex != -1 ? pointToIndex : pointToInterserct->GetSegmentOfContour(contour);
			x = contour->contour.vertex[i].x;
			y = contour->contour.vertex[i].y;
			if (x == original->x && y == original->y)
			{
				i ++;
				x = contour->contour.vertex[i].x;
				y = contour->contour.vertex[i].y;
			}
			AngleTo = Sourse::GetAngle(original->x, original->y, x, y);
		}
	}
	if (DublicateLine != nullptr)
	{
		DublicateLine->AngleFrom = AngleTo;
		DublicateLine->AngleTo = AngleFrom;
	}
}
#pragma endregion

#pragma region --------------------Zone--------------------------------------------

bool Region::isContained(Zone* zone)
{
	list<Zone*>::iterator itPoint;
	itPoint = find_if(zones.begin(), zones.end(), [zone](Zone* o) -> bool
	{
		return o == zone;
	});
	if (itPoint != zones.end())
	{
		return true;
	}
	return false;
}

Region::~Region()
{
	zones.clear();
	borders.clear();
	for (auto it = linesBorders.begin(); it != linesBorders.end(); ++it)
		delete *it;
	linesBorders.clear();
}
#pragma endregion

#pragma region --------------------LinesBorder--------------------------------------------

Zone::~Zone()
{
	lines.clear();
}

float LinesBorder::CheckLine(gpc_vertex p1, gpc_vertex p2) const
{
	return p1.x * p2.y - p1.y * p2.x;
}

float LinesBorder::GetArea(gpc_vertex a, gpc_vertex b, gpc_vertex c) const
{
	int ab = CheckLine(a, b);
	int bc = CheckLine(b, c);
	int ca = CheckLine(c, a);
	int S = (ab + bc + ca) / 2;
	return S;
}

LinesBorder::~LinesBorder()
{
	lines.clear();
	tops.clear();
}
#pragma endregion

#pragma region --------------------GZContours--------------------------------------------

void GZContour::sortPointIntersect()
{
	auto p = this;
	struct Local
	{
		Local(GZContour* c)
		{
			this->contour_ = c;
		}
		bool operator()(PointIntersect* p1, PointIntersect* p2) const
		{			
			IntersectInfo* i1 = (*p1).GetInfoForContour(contour_);
			IntersectInfo* i2 = (*p2).GetInfoForContour(contour_);
			if ((*i2).segment > (*i1).segment)
			{
				return true;
			}
			if ((*i1).segment == (*i2).segment && (*i1).localArcCoordinate < (*i2).localArcCoordinate)
			{
				return true;
			}
			return false;
		}
		bool flag = false;
		GZContour* contour_;
	};
	if (this->pointsIntersect.size() > 0)	this->pointsIntersect.sort(Local(p));
}

GZContour::GZContour()
{
	contour.num_vertices = 0;
	contour.vertex = nullptr;
}

GZContour::~GZContour()
{
	if (contour.vertex != nullptr) delete contour.vertex;
	pointsIntersect.clear();
}

bool GZContour::ContainsPoint(PointIntersect* p)
{
	for (auto l = pointsIntersect.begin(); l != pointsIntersect.end(); ++l)
	{
		if (*l == p)
		{
			return true;
		}
	}
	return false;
}

GZContours::~GZContours()
{
	for (auto it = contours.begin(); it != contours.end(); ++it)
		delete *it;
}

bool TestAddPoint(gpc_vertex* v, int j, FLOATTYPE x, FLOATTYPE y)
{
	if (j < 0) return true;
	return v[j].x != x || v[j].y != y;
}

void GZContours::AddContour(int typeCont, int num, gpc_vertex_list* cont)
{
	GZContour* c = new GZContour();
	c->typeCont = typeCont;
	c->num = num;
	gpc_vertex* v = new gpc_vertex[cont->num_vertices];
	c->contour.vertex = v;
	int j = -1;
	for (int i = 0; i < cont->num_vertices; i++)
	{
		FLOATTYPE xx, yy;
		if (InvertXY)
		{
			xx = cont->vertex[i].y;
			yy = cont->vertex[i].x;
		}
		else
		{
			xx = cont->vertex[i].x;
			yy = cont->vertex[i].y;
		}
		if (!TestAddPoint(v, j, xx, yy)) continue;
		j++;
		v[j].x = xx;
		v[j].y = yy;
	}
	c->contour.num_vertices = j + 1;
	contours.push_back(c);
}

void GZContours::AddContour(int typeCont, int num, int nPoint, FLOATTYPE* x, FLOATTYPE* y)
{
	vector<gpc_vertex> pointsContourRestruct;

	vector<gpc_vertex> pointsContour;
	for (int i = 0; i < nPoint; i++)
	{
		gpc_vertex point;
		point.x = x[i];
		point.y = y[i];
		pointsContour.push_back(point);
	}
	const FLOATTYPE eps = 1.0e-6;
	RamerDouglasPeucker::RestructPoints(pointsContour, eps, pointsContourRestruct);
	nPoint = pointsContourRestruct.size();
	
	GZContour* c = new GZContour();
	c->typeCont = typeCont;
	c->num = num;
	c->contour.num_vertices = nPoint;
	gpc_vertex* v = new gpc_vertex[nPoint];
	c->contour.vertex = v;
	int j = -1;
	for (int i = 0; i < nPoint; i++)
	{
		FLOATTYPE xx, yy;
		if (InvertXY)
		{
			xx = pointsContourRestruct[i].y;
			yy = pointsContourRestruct[i].x;
		}
		else
		{
			xx = pointsContourRestruct[i].x;
			yy = pointsContourRestruct[i].y;
		}
		/*if (InvertXY)
		{
			xx = y[i];
			yy = x[i];
		}
		else
		{
			xx = x[i];
			yy = y[i];
		}*/

		if (!TestAddPoint(v, j, xx, yy)) continue;
		j++;
		v[j].x = xx;
		v[j].y = yy;
	}
	c->contour.num_vertices = j + 1;
	contours.push_back(c);
}

void GZContours::InitContours()
{
	for (vector<GZContour*>::iterator it = contours.begin(); it != contours.end(); ++it)
	{
		auto itMap = contoursByType.find((*it)->typeCont);
		if (itMap == contoursByType.end())
		{
			vector<GZContour*> v;
			itMap = contoursByType.emplace((*it)->typeCont, v).first;
		}
		vector<GZContour*>& vCont = itMap->second;
		vCont.push_back(*it);
	}
}

void GeoZonesObject::Clear()
{
	simleZones.clear();
	regions.clear();
	//zones.clear();
	pointsIntersect.clear();
	lines.clear();
	for (size_t i = 0; i < c.contours.size(); i++)
	{
		GZContour* currContour = c.contours[i];
		currContour->pointsIntersect.clear();
	}
}
#pragma endregion

#pragma region --------------------GeoZonesObject----------------------------------------

gpc_polygon* GeoZonesObject::MakeZone()
{
	Clear();
	Step1();
	Step2();
	Step3();
	Step4();
	Step5();
	Step6();
	Step7();
	Step8();
	return Step9();
}

PointIntersect* GeoZonesObject::FindPointIntersect(gpc_vertex* p)
{
	list<PointIntersect*>::iterator itPoint;
	itPoint = find_if(pointsIntersect.begin(), pointsIntersect.end(), [p](PointIntersect* o) -> bool
	                  {
		                  return o->x == p->x && o->y == p->y;
	                  });
	if (itPoint != pointsIntersect.end())
	{
		return *itPoint;
	}
	return nullptr;
}

void GeoZonesObject::GetInterserct(GZContour* currContour, GZContour* nextContour)
{
	bool isSame = currContour == nextContour;
	gpc_vertex* result = new gpc_vertex();
	for (size_t j = 0; j < (*currContour).contour.num_vertices - 1; j++)
	{
		gpc_vertex a = (*currContour).contour.vertex[j];
		gpc_vertex b = (*currContour).contour.vertex[j + 1];
		if (a.x == b.x && a.y == b.y) continue;
		size_t kk = isSame ? j + 2 : 0;
		for (size_t k = kk; k < (*nextContour).contour.num_vertices - 1; k++)
		{
			gpc_vertex a1 = (*nextContour).contour.vertex[k];
			gpc_vertex b1 = (*nextContour).contour.vertex[k + 1];

			if (a1.x == b1.x && a1.y == b1.y) continue;

			
			FLOATTYPE r = -1; //локальная дуговая координата для точки пересечения первого отрезка
			FLOATTYPE s = -1; //локальная дуговая координата для точки пересечения второго отрезка
			//поиск точки пересечения двух отрезков
			bool flag = Sourse::Intersection(a, b, a1, b1, result, &r, &s);
			if (((r == 0 && s != 1) || (s == 0 && r != 1)) && currContour != nextContour) continue;
			if (flag)
			{
				PointIntersect* p = FindPointIntersect(result);
				if (p == nullptr)
				{
					p = new PointIntersect();
					(*currContour).pointsIntersect.push_back(p);
					pointsIntersect.push_back(p);
				}
				(*p).InitIntersect(currContour, nextContour, j, k, r, s, result);

				PointIntersect* p1 = FindPointIntersect(result);
				if (p1 == nullptr)
				{
					p1 = new PointIntersect();
					pointsIntersect.push_back(p1);
				}
				if (!(*nextContour).ContainsPoint(p1))
				{
					(*nextContour).pointsIntersect.push_back(p1);
				}
				(*p1).InitIntersect(nextContour, currContour, k, j, s, r, result);
			}
			
		}
	}
	delete result;
}

void GeoZonesObject::GetAllPointsIntersect()
{
	for (size_t i = 0; i < c.useContours.size(); i++)
	{
		GZContour* currContour = c.useContours[i];
		for (size_t l = i; l < c.useContours.size(); l++)
		{
			GZContour* nextContour = c.useContours[l];
			GetInterserct(currContour, nextContour);
		}
		currContour->sortPointIntersect();
	}
}

void GeoZonesObject::AddLineToPointIntersect(PointIntersect* p, Line* line)
{
	if (p != nullptr)
	{
		if (!p->ContainedLine(line))
		{
			p->lines.push_back(line);
		}
	}
}

void GeoZonesObject::GetAllLines()
{
	for (auto contour = c.useContours.begin(); contour != c.useContours.end(); ++contour)
	{
		GZContour* currContour = *contour;

		vector<IntersectInfo * > intersections;

		for (auto p = (*currContour).pointsIntersect.begin(); p != (*currContour).pointsIntersect.end(); ++p)
		{
			for (auto info = (*p)->intersections.begin(); info != (*p)->intersections.end(); ++info)
			{
				if ((*info)->contour == currContour)
				{
					intersections.push_back((*info));
				}
			}
		}		
		if (intersections.size() > 1)
		{
			sort(intersections.begin(), intersections.end(),
				[](const IntersectInfo * a, const IntersectInfo * b) -> bool
			{
				if (a->segment < b->segment)
					return true;
				if (a->segment == b->segment && a->localArcCoordinate < b->localArcCoordinate)
					return true;
				return false;
			});

			for (size_t i = 0; i < intersections.size() - 1; i++)
			{
				auto curr = intersections[i];
				auto next = intersections[i + 1];
				if ((curr->localArcCoordinate == 1 && next->localArcCoordinate == 0 && curr->contour == next->contour && curr->segment + 1 == next->segment) ||
					(curr->localArcCoordinate == 0 && next->localArcCoordinate == 1 && curr->contour == next->contour && curr->segment - 1 == next->segment))
				{
					continue;
				}
				int from = curr->segment + 1;
				int to = next->segment;
				if (from-1 == to)
				{
					from = -1;
					to = -1;
				}
				if (to < from)
				{
					to = from;
				}
				Line* line = new Line(currContour, curr->Parent, next->Parent, from, to, false);
				AddLineToPointIntersect(curr->Parent, line);
				AddLineToPointIntersect(next->Parent, line);
				lines.push_back(line);
			}
		}
	}
}

void GeoZonesObject::RemoveLineWithPointsIntersect(bool& isLeastOneSegmentDeleted, Line* a, PointIntersect* p1, PointIntersect* p2, PointIntersect* pointDeleted)
{
	if (p1 != p2)
	{
		p1->lines.remove(a);
		p2->lines.remove(a);
		a->pointFromIntersect = nullptr; //удаляем ссылку на точку Р, у отрезка В
		a->pointToInterserct = nullptr;
		a->isDeleted = true;
		isLeastOneSegmentDeleted = true;
		pointDeleted->isDeleted = true; //удалить точку p (помечаем точку p как удаленную)
	}
}

void GeoZonesObject::RemoveTails()
{
	bool isLeastOneSegmentDeleted; // хотябы один отрезок удален
	do
	{
		isLeastOneSegmentDeleted = false;
		if (lines.size() == 0) // если число оставшихся отрезков = 0
		{
			break;
		}
		for (auto it = lines.begin(); it != lines.end(); ++it) // цикл по всем отрезкам
		{
			Line* a = *it; // текущий отрезок цикла
			PointIntersect* p1 = a->pointFromIntersect; //первый конец текущего отрезка
			PointIntersect* p2 = a->pointToInterserct; //второй конец текущего отрезка
			if (p1->lines.size() == 1) // если в списке отрезков точки Р остался только один отрезок
			{
				RemoveLineWithPointsIntersect(isLeastOneSegmentDeleted, a, p1, p2, p1);
			}
			else if (p1->lines.size() == 0)
			{
				p1->isDeleted = true;
			}
			if (p2->lines.size() == 1) // если в списке отрезков точки Р остался только один отрезок
			{
				RemoveLineWithPointsIntersect(isLeastOneSegmentDeleted, a, p1, p2, p2);
			}
			else if (p2->lines.size() == 0)
			{
				p2->isDeleted = true;
			}
		}
		DeletePointIntersect(); //удаляем сылки на точки и сами точки пересечения, которые отмечены как удаленные
		DeleteLines(); //удаляем отрезки, которые отмечены как удаленные
	} while (isLeastOneSegmentDeleted);
}

void GeoZonesObject::DeletePointIntersect()
{
	for (auto it = lines.begin(); it != lines.end(); ++it)
	{
		auto p = (*it)->pointFromIntersect;
		if (p != nullptr)
		{
			p = p->isDeleted ? nullptr : p;
		}
		auto p1 = (*it)->pointToInterserct;
		if (p1 != nullptr)
		{
			p1 = p1->isDeleted ? nullptr : p1;
		}
	}
	for (auto it = c.useContours.begin(); it != c.useContours.end(); ++it)
	{
		(*it)->pointsIntersect.remove_if([](PointIntersect* n)
			{
				return (*n).isDeleted;
			});
	}
	pointsIntersect.remove_if([](PointIntersect* n)
		{
			return (*n).isDeleted;
		});
}

void GeoZonesObject::DeleteLines()
{
	for (auto it = pointsIntersect.begin(); it != pointsIntersect.end(); ++it)
	{
		(*it)->lines.remove_if([](Line* n)
			{
				return (*n).isDeleted;
			});
	}
	lines.remove_if([](Line* n)
		{
			return (*n).isDeleted;
		});
}

void GeoZonesObject::OverlappingLines()
{
	list<Line*> newLines; //дубли отрезков замыкающих контуров
	for (auto it = lines.begin(); it != lines.end(); ++it)
	{
		Line* line = *it;
		if ((*line).contour->typeUseCont == Closing)
		{
			PointIntersect*& pointTo = (*line).pointToInterserct;
			PointIntersect*& pointFrom = (*line).pointFromIntersect;
			Line* newLine = new Line((*line).contour, pointFrom, pointTo, (*line).pointFromIndex, (*line).pointToIndex, true);
			newLine->DublicateLine = line;
			newLines.push_back(newLine);
			(*line).DublicateLine = newLine;

			AddLineToPointIntersect(pointFrom, newLine);
			if (pointFrom != pointTo) AddLineToPointIntersect(pointTo, newLine);
			(*line).pointFromIntersect = pointFrom;
			(*line).pointToInterserct = pointTo;
		}
	}
	lines.insert(lines.end(), newLines.begin(), newLines.end());
}

void GeoZonesObject::SortLinesForPointIntersects()
{
	for (auto it = pointsIntersect.begin(); it != pointsIntersect.end(); ++it)
	{
		auto p = *it;
		//определяю угол наклона для каждого отрезка
		for (auto it1 = (*p).lines.begin(); it1 != (*p).lines.end(); ++it1)
		{
			(*it1)->SetAngle(p);
		}

		struct Sorting
		{
			Sorting(PointIntersect* p)
			{
				this->point = p;
			}
			bool operator()(const Line* p1, const Line* p2) const
			{
				//значит дубликаты
				if ((*p1).DublicateLine == p2 || (*p2).DublicateLine == p1)
				{
					if ((*p1).pointFromIntersect == (*p2).pointFromIntersect && (*p1).pointToInterserct == (*p2).pointToInterserct)
					{
						return false;
					}
					if ((*p2).pointToInterserct == point)
					{
						return true;
					}
					return false;
				}
				FLOATTYPE angle = (*p1).pointToInterserct == point ? (*p1).AngleTo : (*p1).AngleFrom;
				FLOATTYPE angle1 = (*p2).pointToInterserct == point ? (*p2).AngleTo : (*p2).AngleFrom;
				if (angle < angle1)
				{
					return true;
				}
				return false;
			}
			PointIntersect* point;
		};

		if (p->lines.size() > 1) (*p).lines.sort(Sorting(p));
	}
}

/// <summary> ищем точки пересечения </summary>
void GeoZonesObject::Step1()
{
	GetAllPointsIntersect(); //формирую общий список точек пересечений	
	GetAllLines(); //формирую общий список отрезочков	
	RemoveTails(); //удаляю хвосты
	OverlappingLines(); //дублирую отрезочки для замыкающих контуров
	SortLinesForPointIntersects(); //сортирую отрезки в точке пересечения против часовой стрелки
}

/// <summary> строим набор простых односвязных зон </summary>  
void GeoZonesObject::Step2()
{
	for (auto it = lines.begin(); it != lines.end(); ++it)
	{
		auto line = *it;
		auto D = line;
		if (line->contour->typeUseCont != General) continue; // если отрезок не от основного контура
		if (line->zone != nullptr) continue; // если отрезок уже использовался
		Zone * zone = new Zone(); // создаем новую зону
		zone->typeZone = Closed;
		simleZones.push_back(zone);
		zone->lines.push_back(line);		
		line->zone = zone; // помечаем отрезок как использованный
		PointIntersect * A = line->pointFromIntersect;
		PointIntersect * B = line->pointToInterserct;
		while (A != B)
		{
			Line * C = nullptr;
			for (int i = 0; i < B->lines.size(); i++)
			{
				list<Line*>::iterator iter = B->lines.begin();
				advance(iter, i);
				
				if (*iter == D)
				{
					int index = i + 1;
					if (index == B->lines.size())
					{
						index = 0;
					}
					list<Line*>::iterator iter1 = B->lines.begin();
					advance(iter1, index);
					C = *iter1;
					break;
				}
			}			
			if ((C->pointToInterserct == B && C->contour->typeUseCont == General) || (C->zone != nullptr))
			{
				zone->typeZone = NotClosed;
				break;
			}
			zone->lines.push_back(C);
			C->zone = zone;
			B = C->pointToInterserct;
			D = C;
		}
	}
}

Zone* GeoZonesObject::GetZoneForLine(Line* line)
{
	Zone * B = nullptr;
	for (auto it2 = simleZones.begin(); it2 != simleZones.end(); ++it2)
	{
		auto zone = *it2;
		for (auto it3 = zone->lines.begin(); it3 != zone->lines.end(); ++it3)
		{
			if (*it3 == line->DublicateLine)
			{
				B = *it2;
				break;
			}
			if (B != nullptr) break;
		}
	}
	return B;
}

/// <summary> строим из набора простых односвязных зон замкнутые области (многосвязные) </summary>  
void GeoZonesObject::Step3()
{
	for (auto it = simleZones.begin(); it != simleZones.end(); ++it)
	{
		auto A = *it;
		if (A->typeZone == NotClosed) continue;
		if (A->region != nullptr) continue;
		for (auto it1 = A->lines.begin(); it1 != A->lines.end(); ++it1)
		{
			auto line = *it1;
			if (line->DublicateLine == nullptr) continue;
			Zone * B = GetZoneForLine(line);
			if (B == nullptr) continue;
			if (B->region != nullptr)
			{
				B->region->zones.push_back(A);
				A->region = B->region;
				break;
			}			
		}
		if (A->region == nullptr)
		{
			Region * Q = new Region();
			regions.push_back(Q);
			Q->zones.push_back(A);
			A->region = Q;
		}
	}
}

/// <summary> строим список отрезков границы каждой области </summary>  
void GeoZonesObject::Step4()
{
	for (auto it = regions.begin(); it != regions.end(); ++it)
	{
		auto region = *it;
		for (auto it1 = region->zones.begin(); it1 != region->zones.end(); ++it1)
		{
			auto zone = *it1;
			for (auto it2 = zone->lines.begin(); it2 != zone->lines.end(); ++it2)
			{
				auto line = *it2;
				if (line->DublicateLine != nullptr)
				{
					if (line->DublicateLine->zone != nullptr)
					{
						if (line->DublicateLine->zone->region == region) continue;
					}
				}
				region->borders.push_back(line);
			}
		}
	}
}

/// <summary> соединяем отрезки, составляющие границу, в линии границы </summary>  
void GeoZonesObject::Step5()
{
	for (auto it = regions.begin(); it != regions.end(); ++it)
	{
		auto region = *it;
		for (auto it1 = region->borders.begin(); it1 != region->borders.end(); ++it1)
		{
			auto A = *it1;
			if (A->linesBorder != nullptr) continue;
			LinesBorder * L = new LinesBorder();
			region->linesBorders.push_back(L);
			L->lines.push_back(A);
			A->linesBorder = L;
			L->typeLines = Closed;
			auto B = A;
			while (A->pointFromIntersect != B->pointToInterserct)
			{
				bool isLineAdded = false;
				for (auto it2 = region->borders.begin(); it2 != region->borders.end(); ++it2)
				{
					auto C = *it2;
					if (C->linesBorder != nullptr) continue;
					if (C->pointFromIntersect == B->pointToInterserct)
					{
						L->lines.push_back(C);
						C->linesBorder = L;
						B = C;
						isLineAdded = true;
						break;
					}					
				}
				if (!isLineAdded)
				{
					/*если вошли сюда, значит где-то ошибка!*/
					L->typeLines = NotClosed;
					break;
				}
			}
		}
	}
}

/// <summary> во всех областях удаляем незамкнутые линии границы </summary>  
void GeoZonesObject::Step6()
{
	for (auto it = regions.begin(); it != regions.end(); ++it)
	{
		auto region = *it;
		//во всех областях удаляем незамкнутые линии границы
		region->linesBorders.remove_if([](LinesBorder* n)
		{
			return (*n).typeLines == NotClosed;
		});
		//и линии границы, у которых все отрезки замыкающего типа
		region->linesBorders.remove_if([](LinesBorder* n)
		{
			bool flag = false;
			for (auto it1 = n->lines.begin(); it1 != n->lines.end(); ++it1)
			{
				auto line = *it1;
				if (line->contour->typeUseCont == General)
				{
					flag = true;
					break;
				}
			}
			return flag == false;
		});
	}
}

/// <summary> во всех областях для каждой линии строим массив координат вершин </summary>  
void GeoZonesObject::Step7()
{
	for (auto it = regions.begin(); it != regions.end(); ++it)
	{
		auto region = *it;
		for (auto it1 = region->linesBorders.begin(); it1 != region->linesBorders.end(); ++it1)
		{
			auto linesBorder = *it1;
			int size = linesBorder->lines.size();
			for (auto it2 = linesBorder->lines.begin(); it2 != linesBorder->lines.end(); ++it2)
			{
				size--;
				auto line = *it2;
				gpc_vertex point1;
				point1.x = line->pointFromIntersect->x;
				point1.y = line->pointFromIntersect->y;
				if (linesBorder->tops.size() > 0)
				{
					if (linesBorder->tops.back().x == point1.x && linesBorder->tops.back().y == point1.y) continue;
				}
				linesBorder->tops.push_back(point1);
				if (line->pointFromIndex != -1 && line->pointToIndex != -1)
				{					
					if (line->pointFromIndex > line->pointToIndex)
					{
						for (int i = line->pointFromIndex; i >= line->pointToIndex; i--)
						{
							gpc_vertex point2;
							point2.x = line->contour->contour.vertex[i].x;
							point2.y = line->contour->contour.vertex[i].y;
							linesBorder->tops.push_back(point2);
						}
					}
					else
					{
						for (int i = line->pointFromIndex; i <= line->pointToIndex; i++)
						{
							gpc_vertex point2;
							point2.x = line->contour->contour.vertex[i].x;
							point2.y = line->contour->contour.vertex[i].y;
							linesBorder->tops.push_back(point2);
						}
					}
				}
			}
		}
	}
}

bool GeoZonesObject::isHole(LinesBorder * line, bool isUse)
{
	for (auto region = regions.begin(); region != regions.end(); ++region)
	{
		for (auto l = (*region)->linesBorders.begin(); l != (*region)->linesBorders.end(); ++l)
		{
			if (line == *l || (*l)->isUse == isUse) continue;

			//проверяем входит ли полигон полностью в какой-либо другой
			bool flag = false;
			for (auto l1 = (*line).tops.begin(); l1 != (*line).tops.end(); ++l1)
			{
				if (Sourse::IsInside((*l)->tops, (*l1).x, (*l1).y))
				{
					flag = true;
				}
				else
				{
					flag = false;
					break;
				}
			}
			if (flag)
			{
				(*l)->isUse = true;
				(*line).parent = *l;
				return true;
			}
		}
	}
	return false;
}

/// <summary> для всех линий границ определяем признак контур/дырка </summary>  
void GeoZonesObject::Step8()
{
	for (auto region = regions.begin(); region != regions.end(); ++region)
	{
		for (auto lineBorder = (*region)->linesBorders.begin(); lineBorder != (*region)->linesBorders.end(); ++lineBorder)
		{
			FLOATTYPE area = 0;
			gpc_vertex a;
			a.x = (*lineBorder)->tops[0].x;
			a.y = (*lineBorder)->tops[0].y;
			gpc_vertex b, c;
			for (size_t i = 2; i < (*lineBorder)->tops.size() - 1; i++)
			{				
				b.x = (*lineBorder)->tops[i-1].x;
				b.y = (*lineBorder)->tops[i-1].y;

				c.x = (*lineBorder)->tops[i].x;
				c.y = (*lineBorder)->tops[i].y;

				FLOATTYPE s = (*lineBorder)->GetArea(a, b, c);
				area += s;
			}
			//дополнительное условие <(*lineBorder)->lines.size() != 1> необходимо для того, чтобы не считать дырками замкнутые контура, 
			//которые не имеют пересечений с другими контурами, иначе приходилось бы перенумеровывать подоюные контура по часовой стрелке
			//bool flag = (*lineBorder)->lines.size() == 1 &&
			//	((*lineBorder)->lines[0]->contour->typeCont == 4 || (*lineBorder)->lines[0]->contour->typeCont == 6);

			//if (flag)
			//{
				bool flag1 = isHole(*lineBorder, true) == true ? true : isHole(*lineBorder, false);				
				(*lineBorder)->hole = flag1 && (*lineBorder)->parent->hole == false;
			//}
			//else
			//{
			//	(*lineBorder)->hole = area > 0;
			//}
		}
	}
}

/// <summary> создаем результирующий полигон </summary>
gpc_polygon* GeoZonesObject::Step9()
{
	vector<gpc_polygon*> polygons;
	vector<gpc_polygon*> holePolygons;

	for (auto region = regions.begin(); region != regions.end(); ++region)
	{
		for (auto lineBorder = (*region)->linesBorders.begin(); lineBorder != (*region)->linesBorders.end(); ++lineBorder)
		{
			int n = (*lineBorder)->tops.size();
			gpc_polygon * polygon = Sourse::newPolygon();
			gpc_vertex_list* cont = new gpc_vertex_list;
			cont->num_vertices = n;
			cont->vertex = new gpc_vertex[n];
			if (!(*lineBorder)->hole)
			{
				for (size_t i = 0; i < n; i++)
				{
					cont->vertex[i].x = (*lineBorder)->tops[i].x;
					cont->vertex[i].y = (*lineBorder)->tops[i].y;
				}
				polygons.push_back(polygon);
			}
			else
			{
				for (size_t i = 0; i < n; i++)
				{
					cont->vertex[i].x = (*lineBorder)->tops[n - 1 - i].x;
					cont->vertex[i].y = (*lineBorder)->tops[n - 1 - i].y;
				}
				holePolygons.push_back(polygon);
			}
			gpc_add_contour(polygon, cont, (*lineBorder)->hole);
			delete[] cont->vertex;
			delete cont;
		}
	}
	gpc_polygon* prev = Sourse::newPolygon();
	for (size_t i = 0; i < polygons.size(); i++)
	{
		gpc_polygon* curr = polygons[i];
		gpc_polygon* result = Sourse::newPolygon();
		gpc_polygon_clip(GPC_UNION, prev, curr, result);
		Sourse::freePolygon(prev);
		prev = result;
	}
	for (size_t i = 0; i < holePolygons.size(); i++)
	{
		gpc_polygon* curr = holePolygons[i];
		gpc_polygon* result = Sourse::newPolygon();
		gpc_polygon_clip(GPC_DIFF, prev, curr, result);
		Sourse::freePolygon(prev);
		prev = result;
	}
	
	if (prev->num_contours > 0)
	{
#pragma region Обработка случая <остров в дырке>
		for (size_t i = 0; i < polygons.size(); i++)
		{
			bool flag = false;
			gpc_polygon* curr = polygons[i];
			for (size_t j = 0; j < curr->contour->num_vertices; j++)
			{
				vector<gpc_vertex> tops;
				for (size_t l = 0; l < prev->contour->num_vertices; l++)
				{
					tops.push_back(prev->contour->vertex[l]);
				}

				gpc_vertex vertex = curr->contour->vertex[j];
				if (Sourse::IsInside(tops, vertex.x, vertex.y))
				{
					flag = true;
				}
				else
				{
					flag = false;
					break;
				}
			}

			if (flag)
			{
				gpc_polygon* result = Sourse::newPolygon();
				gpc_polygon_clip(GPC_UNION, prev, curr, result);
				Sourse::freePolygon(prev);
				prev = result;
			}
		}
#pragma endregion
	}
	return prev;
}

/// <summary> Деструктор полигона зон </summary>  
GeoZonesObject::~GeoZonesObject()
{
	for (auto it = zones.begin(); it != zones.end(); ++it)
		Sourse::freePolygon((*it).second);
	for (auto it = pointsIntersect.begin(); it != pointsIntersect.end(); ++it)
		delete *it;
	for (auto it = lines.begin(); it != lines.end(); ++it)
		delete *it;
	for (auto it = regions.begin(); it != regions.end(); ++it)
		delete *it;
	for (auto it = simleZones.begin(); it != simleZones.end(); ++it)
		delete *it;
}

/// <summary> Метод получения сетки как дополнительной зоны </summary> 
void GeoZonesObject::GetRect(gpc_polygon*& zone)
{
	GZContour * rect = nullptr;
	for (auto it = c.contours.begin(); it < c.contours.end(); ++it)
	{
		if ((*it)->typeCont == -1)
		{
			rect = *it;
			break;
		}
	}

	gpc_polygon * polygon = Sourse::newPolygon();
	if (rect != nullptr)
	{
		gpc_vertex_list* cont = new gpc_vertex_list;
		cont->num_vertices = rect->contour.num_vertices;
		cont->vertex = new gpc_vertex[rect->contour.num_vertices];
		for (int i = 0; i < rect->contour.num_vertices; i++)
		{
			cont->vertex[i].x = rect->contour.vertex[i].x;
			cont->vertex[i].y = rect->contour.vertex[i].y;
		}
		gpc_add_contour(polygon, cont, 0);
		delete[] cont->vertex;
		delete cont;
	}
	zone = polygon;
}

/// <summary> Возвращает готовую замкнутую зону
/// <param name = "typeCont">тип контура</param>
/// <param name = "zone">зона</param>
/// </summary>  
void GeoZonesObject::GetZone(int typeZone, gpc_polygon*& zone)
{
	auto itZones = zones.find(typeZone);
	if (itZones != zones.end())
	{
		zone = (*itZones).second;
		return;
	}
	zone = Sourse::newPolygon();

	vector<int> typeZoneForMinus;
	switch (typeZone)
	{
	case 1: // водяная	
			typeZoneForMinus.push_back(2);
	case 2: //водонефтяная зона
			typeZoneForMinus.push_back(3);
	case 3: //чистонефтяная зона	
			typeZoneForMinus.push_back(4);	
	case 4: //подгазовая зона	
			typeZoneForMinus.push_back(5);
	case 5: //газовая зона
	case 6: // зона распространения коллектора		
			typeZoneForMinus.push_back(13);
	case 11: // зона отсутствия коллектора без учета линий замещения
			typeZoneForMinus.push_back(14);
			break;
	case 12: // зона отсутствия коллектора без учета линий выклинивания
			typeZoneForMinus.push_back(13);
			break;
	case 8: // эффективная зона пласта
		typeZoneForMinus.push_back(2);
		typeZoneForMinus.push_back(3);
		typeZoneForMinus.push_back(4);
		typeZoneForMinus.push_back(5);
		break;
	case 9: //зона нефтеностности
		typeZoneForMinus.push_back(2);
		typeZoneForMinus.push_back(3);
		break;
	case 10: //зона газоностности	
		typeZoneForMinus.push_back(4);
		typeZoneForMinus.push_back(5);
		break;
	case 7: // зона отсутствия коллектора
		typeZoneForMinus.push_back(13);
		typeZoneForMinus.push_back(14);
		break;
	case 13: //замещения
	case 14: //выклинивания
			break;
	default:
			return;
	}

	auto op = GPC_DIFF;

	if (typeZone == 1 || typeZone == 6 || typeZone == 11 || typeZone == 12)
	{
		GetRect(zone);
	}
	else if (typeZone == 8 || typeZone == 9 || typeZone == 10 || typeZone == 7)
	{
		op = GPC_UNION;
	}
	else
	{
		GetZoneBase(typeZone, zone, false);
	}	

	if (c.InvertXY) TransformCoordinatePoints(zone);

	for (auto it = typeZoneForMinus.begin(); it < typeZoneForMinus.end(); ++it)
	{
		int type = *it;
		gpc_polygon* zoneMinus = nullptr;
		GetZone(type, zoneMinus);
		gpc_polygon * result = Sourse::newPolygon();
		gpc_polygon_clip(op, zone, zoneMinus, result);
		Sourse::freePolygon(zone);
		zone = result;
	}

	zones.emplace(typeZone, zone);
}

/// <summary> Возвращает готовую замкнутую зону
/// <param name = "typeCont">тип контура</param>
/// <param name = "zone">зона</param>
/// <param name = "secondEnter">повторный ввод</param>
/// </summary>  
void GeoZonesObject::GetZoneBase(int typeZone, gpc_polygon*& zone, bool secondEnter)
{	
	//главные контуры
	set<int> mainTypeContours;

	switch (typeZone)
	{
	case 2: //водонефтяная зона
	case 9: //зона нефтяностная
		mainTypeContours.emplace(0);
		break;
	case 3: //чистонефтяная зона
		mainTypeContours.emplace(1);
		break;
	case 4: //подгазовая зона
	case 10: //зона газоностная
		mainTypeContours.emplace(2);
		break;
	case 5: //газовая зона
		mainTypeContours.emplace(3);
		break;

	case 13: //замещения
		mainTypeContours.emplace(4);
		break;
	case 14: //выклинивания
		mainTypeContours.emplace(6);
		break;
	default:
		return;
	}

	//замыкающие контуры
	set<int> closeTypeContours;
	switch (typeZone)
	{
	case 2: //водонефтяная зона
	case 3: //чистонефтяная зона
	case 4: //подгазовая зона
	case 5: //газовая зона
	case 8: //эффективная зона пласта
	case 9: //зона нефтяностная
	case 10: //зона газоностная
		closeTypeContours.emplace(4);
		closeTypeContours.emplace(5);
		closeTypeContours.emplace(6);
		closeTypeContours.emplace(11);
		closeTypeContours.emplace(21);
		closeTypeContours.emplace(22);
		break;
	case 13: //замещения
	case 14: //выклинивания
		closeTypeContours.emplace(5);
		closeTypeContours.emplace(11);
		closeTypeContours.emplace(21);
		break;
	default: break;
	}

	if (secondEnter)
	{
		closeTypeContours.emplace(10);
		closeTypeContours.emplace(12);
		closeTypeContours.emplace(13);
		closeTypeContours.emplace(14);
		closeTypeContours.emplace(15);
		closeTypeContours.emplace(16);
	}

	c.useMainContours.clear();
	c.useClosedContours.clear();
	c.useContours.clear();

	for (vector<GZContour*>::iterator it = c.contours.begin(); it != c.contours.end(); ++it)
	{
		auto itTypeCont = mainTypeContours.find((*it)->typeCont);
		if (itTypeCont != mainTypeContours.end())
		{
			(*it)->typeUseCont = General;
			c.useMainContours.push_back(*it);
		}
		else
		{
			itTypeCont = closeTypeContours.find((*it)->typeCont);
			if (itTypeCont != closeTypeContours.end())
			{
				(*it)->typeUseCont = Closing;
				c.useClosedContours.push_back(*it);
			}
			else (*it)->typeUseCont = None;
		}
		if ((*it)->typeUseCont != None) c.useContours.push_back(*it);
	}

	if (c.useMainContours.size() == 0)
	{
		zone = Sourse::newPolygon();
	}
	else
	{
		zone = MakeZone();	
	}

	//если не удалось замкнуть зону
	if (zone->num_contours == 0 && secondEnter == false)
	{
		GetZoneBase(typeZone, zone, true);
	}
}

void GeoZonesObject::TransformCoordinatePoints(gpc_polygon* p)
{
	for (int j = 0; j < p->num_contours; j++)
	{
		auto currContour = p->contour[j];
		for (size_t i = 0; i < (currContour).num_vertices; i++)
		{
			FLOATTYPE temp = (currContour).vertex[i].y;
			(currContour).vertex[i].y = (currContour).vertex[i].x;
			(currContour).vertex[i].x = temp;
		}
	}
}

bool GeoZonesObject::ContainsLine(Line* line)
{
	for (auto l = lines.begin(); l != lines.end(); ++l)
	{
		if ((*l)->contour == line->contour &&
			(*l)->pointFromIntersect == line->pointFromIntersect &&
			(*l)->pointToInterserct == line->pointToInterserct &&
			(*l)->pointFromIndex == line->pointFromIndex &&
			(*l)->pointToIndex == line->pointToIndex)
		{
			return true;
		}
	}
	return false;
}
#pragma endregion
