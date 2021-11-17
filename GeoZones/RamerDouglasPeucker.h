#include <vector>
#include "gpc.h"

static class RamerDouglasPeucker
{

	RamerDouglasPeucker()
	{
	}

	virtual ~RamerDouglasPeucker()
	{
	}

	//"findMaximumDistance" used as part of implementation for RDP algorithm.
private:
	static double PerpendicularDistance(const gpc_vertex &pt, const gpc_vertex &lineStart, const gpc_vertex &lineEnd);

	//"simplifyWithRDP" returns the simplified path with a Point vector. The function takes in the paths to be simplified and a customerized thresholds for the simplication.
public:
	static void RestructPoints(const std::vector<gpc_vertex> &pointList, FLOATTYPE epsilon, std::vector<gpc_vertex> &out);
};
