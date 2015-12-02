#pragma once
#include <vector>
using namespace std;
#include "acis_headers.h"

typedef SPAposition PolyPoint;
typedef SPAvector PolyVector;
typedef SPAvector PolyNormal;

double polyline_similarity (vector<PolyPoint> &polyline1, vector<PolyPoint> &polyline2, PolyNormal normal, double epsilon);
double maximum_distance (vector<PolyPoint> &polyline1, vector<PolyPoint> &polyline2);

double point_line_distance_squared (const PolyPoint &q, const PolyPoint &line_start, const PolyVector &line_vec, double &t);
double point_lineseg_distance_squared (const PolyPoint &q, const PolyPoint &lineseg_start, const PolyPoint &lineseg_end);
double point_polyline_distance_squared (const PolyPoint &p, vector<PolyPoint> &polyline);
