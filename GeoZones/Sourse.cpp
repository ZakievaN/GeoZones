#include <complex>
#include "Sourse.h"


const FLOATTYPE PI = asin(1.0) * 2;

FLOATTYPE min(FLOATTYPE x, FLOATTYPE y)
{
	return x < y ? x : y;
}

FLOATTYPE max(FLOATTYPE x, FLOATTYPE y)
{
	return x > y ? x : y;
}

/// <summary> Конструктор зоны
/// </summary>  
gpc_polygon* Sourse::newPolygon()
{
	gpc_polygon* p = new gpc_polygon();
	p->num_contours = 0;
	p->contour = nullptr;
	p->hole = nullptr;
	return p;
}

void Sourse::freePolygon(gpc_polygon*& p)
{
	gpc_free_polygon(p);
	delete p;
	p = nullptr;
}

/// <summary> алгоритм из пособия, пункт 1.03 </summary>  
bool Sourse::Intersection(gpc_vertex a, gpc_vertex b, gpc_vertex c, gpc_vertex d, gpc_vertex * p, FLOATTYPE * r, FLOATTYPE * s)
{
	
#pragma region определение факта пересечения отрезков с помощью ограничивающих прямоугольников
	/*//левый нижний угол ограничивающего прямоугольника
	FLOATTYPE Xa = min(a.x, b.x);
	FLOATTYPE Ya = min(a.y, b.y);
	//правый верхний угол ограничивающего прямоугольника
	FLOATTYPE Xb = max(a.x, b.x);
	FLOATTYPE Yb = max(a.y, b.y);

	//левый нижний угол ограничивающего второго прямоугольника
	FLOATTYPE Xc = min(c.x, d.x);
	FLOATTYPE Yc = min(c.y, d.y);
	//правый верхний угол ограничивающего второго прямоугольника
	FLOATTYPE Xd = max(c.x, d.x);
	FLOATTYPE Yd = max(c.y, d.y);

	if (!(Xb >= Xc && Xd >= Xa && Yb >= Yc && Yd >= Ya)) return false;*/

	//по результатам диагностики, этот кусок кода, заметно увеличивает время работы алгоритма.
#pragma endregion	
	

	FLOATTYPE denominator = (b.x - a.x)*(d.y - c.y) - (b.y - a.y)*(d.x - c.x);
	const FLOATTYPE eps = 1.0e-6;
	//значит отрезки параллельны
	if (denominator == 0)
	{
		complex<FLOATTYPE> ab(b.x - a.x, b.y - a.y);
		complex<FLOATTYPE> ac(c.x - a.x, c.y - a.y);
		FLOATTYPE absab = abs(ab);
		if (absab < eps) return false;
		FLOATTYPE v = imag(conj(ab) * ac) / absab;
		if (abs(v) > eps) return false; //отрезки не лежат на одной прямой
		//C
		FLOATTYPE rc = abs(ac) / absab;
		v = real(conj(ab) * ac);
		if (v < 0) rc = -rc;
		//D
		complex<FLOATTYPE> ad(d.x - a.x, d.y - a.y);
		FLOATTYPE rd = abs(ad) / absab;
		v = real(conj(ab) * ad);
		if (v < 0) rd = -rd;
		
		complex<FLOATTYPE> cd(d.x - c.x, d.y - c.y);
		complex<FLOATTYPE> ca(a.x - c.x, a.y - c.y);
		FLOATTYPE abscd = abs(cd);
		if (abscd < eps) return false;
		//A
		FLOATTYPE sa = abs(ca) / abscd;
		v = real(conj(cd) * ca);
		if (v < 0) sa = -sa;
		//B
		complex<FLOATTYPE> cb(b.x - c.x, b.y - c.y);
		FLOATTYPE sb = abs(cb) / abscd;
		v = real(conj(cd) * cb);
		if (v < 0) sb = -sb;

		FLOATTYPE rmin = min(rc, rd);
		FLOATTYPE rmax = max(rc, rd);

		FLOATTYPE smin = min(sa, sb);
		FLOATTYPE smax = max(sa, sb);

		if (rmin <= 1 && rmax >= 0 && smin <= 1 && smax >= 0) //значит есть общий участок
		{
			rmin = max(0, rmin);
			rmax = min(1, rmax);
			(*r) = (rmin + rmax) * 0.5f;
			smin = max(0, smin);
			smax = min(1, smax);
			(*s) = (smin + smax) * 0.5f;
			(*p).x = a.x + *r*(b.x - a.x);
			(*p).y = a.y + *r*(b.y - a.y);
			return true;
		}
		return false;
	}	

	*r = ((a.y - c.y)*(d.x - c.x) - (a.x - c.x)*(d.y - c.y)) / denominator;
	*s = ((a.y - c.y)*(b.x - a.x) - (a.x - c.x)*(b.y - a.y)) / denominator;
	bool isIntersect = (*r >= 0 && *r <= 1) && (*s >= 0 && *s <= 1);
	if (!isIntersect)
	{
		return false;
	}

	(*p).x = a.x + *r*(b.x - a.x);
	(*p).y = a.y + *r*(b.y - a.y);

	return true;
}

FLOATTYPE Sourse::GetAngle(FLOATTYPE x, FLOATTYPE y, FLOATTYPE x1, FLOATTYPE y1)
{
	//определяю в какой четверти, относительно первой точки, находится вторая.
	FLOATTYPE yy = y - y1;
	FLOATTYPE xx = x - x1;
	FLOATTYPE angle;
	if (x == x1 && y1 > y) return 90;
	if (abs(x) == abs(x1)) return 270;
	angle = (atan2(yy, xx) * (180 / PI)) + 180;
	if (angle == 360) return 0;
	return angle;
}

bool Sourse::IsInside(vector<gpc_vertex> tops, float x, float y)
{
	bool flag = false;
	for (int i = 0, j = tops.size() - 1; i < tops.size(); j = i++)
	{
		if ((((tops[i].y <= y) && (y<tops[j].y)) || ((tops[j].y <= y) && (y<tops[i].y))) &&
			(x >(tops[j].x - tops[i].x) * (y - tops[i].y) / (tops[j].y - tops[i].y) + tops[i].x))
			flag = !flag;
	}
	return flag;
}
