#include <cmath>
#include <utility>
#include <vector>
#include <stdexcept>
#include "RamerDouglasPeucker.h"

using namespace std;

typedef std::pair<double, double> Point;

double RamerDouglasPeucker::PerpendicularDistance(const gpc_vertex &pt, const gpc_vertex &lineStart, const gpc_vertex &lineEnd)
{
	double dx = lineEnd.x - lineStart.x;
	double dy = lineEnd.y - lineStart.y;

	//Normalise
	double mag = pow(pow(dx, 2.0) + pow(dy, 2.0), 0.5);
	if (mag > 0.0)
	{
		dx /= mag; dy /= mag;
	}

	double pvx = pt.x - lineStart.x;
	double pvy = pt.y - lineStart.y;

	//Get dot product (project pv onto normalized direction)
	double pvdot = dx * pvx + dy * pvy;

	//Scale line direction vector
	double dsx = pvdot * dx;
	double dsy = pvdot * dy;

	//Subtract this from pv
	double ax = pvx - dsx;
	double ay = pvy - dsy;

	return pow(pow(ax, 2.0) + pow(ay, 2.0), 0.5);
}

void RamerDouglasPeucker::RestructPoints(const std::vector<gpc_vertex> &pointList, FLOATTYPE epsilon, std::vector<gpc_vertex> &out)
{
	if (pointList.size()<2)
		throw invalid_argument("Not enough points to simplify");

	// Find the point with the maximum distance from line between start and end
	double dmax = 0.0;
	size_t index = 0;
	size_t end = pointList.size() - 1;
	for (size_t i = 1; i < end; i++)
	{
		double d = RamerDouglasPeucker::PerpendicularDistance(pointList[i], pointList[0], pointList[end]);
		if (d > dmax)
		{
			index = i;
			dmax = d;
		}
	}

	// If max distance is greater than epsilon, recursively simplify
	if (dmax > epsilon)
	{
		// Recursive call
		vector<gpc_vertex> recResults1;
		vector<gpc_vertex> recResults2;
		vector<gpc_vertex> firstLine(pointList.begin(), pointList.begin() + index + 1);
		vector<gpc_vertex> lastLine(pointList.begin() + index, pointList.end());
		RestructPoints(firstLine, epsilon, recResults1);
		RestructPoints(lastLine, epsilon, recResults2);

		// Build the result list
		out.assign(recResults1.begin(), recResults1.end() - 1);
		out.insert(out.end(), recResults2.begin(), recResults2.end());
		if (out.size()<2)
			throw runtime_error("Problem assembling output");
	}
	else
	{
		//Just return start and end points
		out.clear();
		out.push_back(pointList[0]);
		out.push_back(pointList[end]);
	}
}