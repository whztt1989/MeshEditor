#include "stdafx.h"
#include <assert.h>
#include <cmath>
#include <vector>
using namespace std;
#include "polysim.h"

/************************************************************************/
/* Based on the paper: Qualitative Polyline Similarity Testing with 
         Applications to Query-by-Sketch, Indexing and Classification                                                                     */
/************************************************************************/

class Double_Cross_Tuple
{
public:
	Double_Cross_Tuple (int c1, int c2, int c3, int c4)
	{
		C1 = c1; C2 = c2; C3 = c3; C4 = c4;
	}
	Double_Cross_Tuple ()
	{
		C1 = C2 = C3 = C4 = 0;
	}
	~Double_Cross_Tuple(){}
public:
	int C1, C2, C3, C4;
};

class Double_Cross_Matrix
{
public:
	Double_Cross_Matrix (const vector<PolyPoint> &polyline)
	{
		N = polyline.size ();
		m_matrix.resize (N);
		for (size_t i = 0; i < N; ++i)
			m_matrix[i].resize (N);
	}
	Double_Cross_Matrix (size_t n) 
	{
		N = n;
		m_matrix.resize (N);
		for (size_t i = 0; i < N; ++i)
			m_matrix[i].resize (N);
	}
	~Double_Cross_Matrix()
	{
	}
public:
	Double_Cross_Tuple & at (int i, int j)
	{
		return m_matrix[i][j];
	}
	size_t get_N (){return N;}
private:
	size_t N;
	vector<vector<Double_Cross_Tuple> > m_matrix;
};

int DC_distance (const Double_Cross_Tuple &dc1, const Double_Cross_Tuple &dc2)
{
	return abs (dc1.C1 - dc2.C1) + abs (dc1.C2 - dc2.C2) + 
		abs (dc1.C3 - dc2.C3) + abs (dc1.C4 - dc2.C4);
}

int sign (double val)
{
	if (val > 0)
		return 1;
	else if (val == 0)
		return 0;
	else
		return -1;
}

//len_table: array stores accumulated length, 0, lP1, lP1 + lP2, lP1 + lP2 + lP3, ..., lP1 + lP2 +...+ lPn
void calc_Nth_polyline (const vector<PolyPoint> &origin_polyline, 
						double N, 
						const vector<double> &len_table, 
						vector<PolyPoint> &Nth_polyline)
{
	assert (N >= 0);
	if (N == 1)
	{
		Nth_polyline = origin_polyline;	return;
	}
	//total length of the original polyline
	double lP = len_table.back ();
	int cur_len_table_idx = 0;
	int n_points = (int)(origin_polyline.size ());
	for (int i = 0; i < N; ++i)
	{
		double len = i * lP / N;
		for (int j = cur_len_table_idx; j < n_points - 1; ++j)
		{
			if (len_table[j] <= len && len_table[j + 1] > len)
			{
				cur_len_table_idx = j;
				break;
			}
		}//end for (j = current_idx_in_table; j < n_poin ...
		const PolyPoint &p1 = origin_polyline[cur_len_table_idx];
		const PolyPoint &p2 = origin_polyline[cur_len_table_idx + 1];
		PolyVector v = p2 - p1;
		double lPi = v.len ();
		double len_ratio = (len - len_table[cur_len_table_idx]) / lPi;
		PolyPoint np = v * len_ratio + p1;
		Nth_polyline.push_back (np);
	}//end for (int i = 0; i <= N; ++i)...
	//push back the last point in origin_polyline
	Nth_polyline.push_back (origin_polyline.back ());
}

void calc_len_table (const vector<PolyPoint> &polyline, vector<double> &len_table)
{
	len_table.clear (); len_table.push_back (0.0f);
	double totoal_len = 0.0f;
	for (size_t i = 1; i < polyline.size (); ++i)
	{
		totoal_len += (polyline[i] - polyline[i - 1]).len ();
		len_table.push_back (totoal_len);
	}
}

void calc_double_crosss_tuple (const PolyPoint &Pi_1, const PolyPoint &Pi, 
							   const PolyPoint &Pj_1, const PolyPoint &Pj,
							   const PolyNormal &w,
							   Double_Cross_Tuple &tuple)
{
	PolyVector u = Pj_1 - Pi_1;
	PolyVector li_apos = Pi - Pi_1,
		lj_apos = Pj - Pj_1;
	tuple.C1 = -sign (u % li_apos);
	tuple.C2 = sign (u % lj_apos);
	tuple.C3 = -sign ((u * li_apos) % w);
	tuple.C4 = sign ((u * lj_apos) % w);
}

void calc_double_cross_matrix (const vector<PolyPoint> &polyline, const PolyVector &w, Double_Cross_Matrix ** dcm)
{
	//if a polyline has M nodes, then it has N = M - 1 lines
	size_t N = polyline.size () - 1;
	(*dcm) = new Double_Cross_Matrix (N);
	for (size_t i = 0; i < N; ++i)
	{
		const PolyPoint &Pi_1 = polyline[i];
		const PolyPoint &Pi = polyline[i + 1];
		for (size_t j = i + 1; j < N; ++j)
		{
			const PolyPoint &Pj_1 = polyline[j];
			const PolyPoint &Pj = polyline[j + 1];
			calc_double_crosss_tuple (Pi_1, Pi, Pj_1, Pj, w, (*dcm)->at (i, j));
		}
	}
}

double calc_distance (Double_Cross_Matrix &m1, Double_Cross_Matrix &m2, PolyVector &w)
{
	double dist = 0.0f;
	size_t N = m1.get_N ();
	for (size_t i = 0; i < N; ++i)
	{
		for (size_t j = i + 1; j < N; ++j)
		{
			dist += DC_distance (m1.at (i, j), m2.at (i, j));
		}
	}
	double divider = 1.0f / (double)((4 * (N - 1) * (N - 1)));
	return dist * divider;
}

double polyline_similarity (vector<PolyPoint> &polyline1, vector<PolyPoint> &polyline2, PolyNormal normal, double epsilon)
{
	vector<double> len_table1, len_table2;
	calc_len_table (polyline1, len_table1);
	calc_len_table (polyline2, len_table2);
	double len1 = len_table1.back (), len2 = len_table2.back ();
	double _comp_len1 = epsilon * len1, _comp_len2 = epsilon * len2;
	vector<PolyPoint> Nth_polyline1, Nth_polyline2;

	int expo = 1;
	double Nth = 0.0f;
	vector<double> Nth_len_table1, Nth_len_table2;
	do 
	{
		Nth = pow ((double)2.0, expo);
		Nth_polyline1.clear (); Nth_polyline2.clear ();
		calc_Nth_polyline (polyline1, Nth, len_table1, Nth_polyline1);
		calc_Nth_polyline (polyline2, Nth, len_table2, Nth_polyline2);
		calc_len_table (Nth_polyline1, Nth_len_table1);
		calc_len_table (Nth_polyline2, Nth_len_table2);
		expo++;
	} while ((fabs (Nth_len_table1.back () - len1) > _comp_len1) || (fabs(Nth_len_table2.back () - len2) > _comp_len2));

	Double_Cross_Matrix *m1 = NULL, *m2 = NULL;
	calc_double_cross_matrix (Nth_polyline1, normal, &m1);
	calc_double_cross_matrix (Nth_polyline2, normal, &m2);
	return calc_distance (*m1, *m2, normal);
}

double point_line_distance_squared (const PolyPoint &q, const PolyPoint &line_start, const PolyVector &line_vec, double &t)
{
	double distance_squared;
	t = line_vec % (q - line_start);
	t /= line_vec % line_vec;
	PolyPoint qPrime;
	qPrime = line_start + t * line_vec;
	PolyVector vec = q - qPrime;
	distance_squared = vec % vec;
	return distance_squared;
}

double point_lineseg_distance_squared (const PolyPoint &q, const PolyPoint &lineseg_start, const PolyPoint &lineseg_end)
{
	double distance_squared;
	double t;
	distance_squared = point_line_distance_squared (q, lineseg_start, lineseg_end - lineseg_start, t);
	if (t < 0)
	{
		PolyVector vec = q - lineseg_start;
		distance_squared = vec % vec;
	}
	else if (t > 1)
	{
		PolyVector vec = q - lineseg_end;
		distance_squared = vec % vec;
	}
	return distance_squared;
}

double point_polyline_distance_squared (const PolyPoint &p, vector<PolyPoint> &polyline)
{
	double dSq;
	double xMinusA, yMinusB, zMinusC;
	double xNextMinusA, yNextMinusB, zNextMinusC;
	double xMinusASq, yMinusBSq, zMinusCSq;
	double xNextMinusASq, yNextMinusBSq, zNextMinusCSq;

	xNextMinusA = polyline[1].x () - p.x ();
	yNextMinusB = polyline[1].y () - p.y ();
	zNextMinusC = polyline[1].z () - p.z ();

	xNextMinusASq = xNextMinusA * xNextMinusA;
	yNextMinusBSq = yNextMinusB * yNextMinusB;
	zNextMinusCSq = zNextMinusC * zNextMinusC;

	dSq = point_lineseg_distance_squared (p, polyline[0], polyline[1]);
	for (size_t i = 1; i < polyline.size () - 1; ++i)
	{
		if (dSq < SPAresabs)
			break;
		xMinusA = xNextMinusA;
		yMinusB = yNextMinusB;
		zMinusC = zNextMinusC;

		xMinusASq = xNextMinusASq;
		yMinusBSq = yNextMinusBSq;
		zMinusCSq = zNextMinusCSq;
		
		xNextMinusA = polyline[i + 1].x () - p.x ();
		yNextMinusB = polyline[i + 1].y () - p.y ();
		zNextMinusC = polyline[i + 1].z () - p.z ();

		xNextMinusASq = xNextMinusA * xNextMinusA;
		yNextMinusBSq = yNextMinusB * yNextMinusB;
		zNextMinusCSq = zNextMinusC * zNextMinusC;

		if (((xMinusASq > dSq) && (xNextMinusASq > dSq) && (xMinusA * xNextMinusA > 0)) ||
			((yMinusBSq > dSq) && (yNextMinusBSq > dSq) && (yMinusB * yNextMinusB > 0)) ||
			((zMinusCSq > dSq) && (zNextMinusCSq > dSq) && (zMinusC * zNextMinusC > 0)))
		{
			continue;
		}
		double new_dSq = point_lineseg_distance_squared (p, polyline[i], polyline[i + 1]);
		if (new_dSq < dSq)
			dSq = new_dSq;
	}
	return dSq;
}

//the maximum distance between polyline1 and polyline2
double maximum_distance (vector<PolyPoint> &polyline1, vector<PolyPoint> &polyline2)
{
	double max_dSq = 0.0f;
	for (size_t i = 0; i < polyline1.size (); ++i)
	{
		const PolyPoint &pt = polyline1[i];
		double dist = point_polyline_distance_squared (pt, polyline2);
		if (dist > max_dSq)
			max_dSq = dist;
	}

	for (size_t i = 0; i < polyline2.size (); ++i)
	{
		const PolyPoint &pt = polyline2[i];
		double dist = point_polyline_distance_squared (pt, polyline1);
		if (dist > max_dSq)
			max_dSq = dist;
	}
	return sqrt (max_dSq);
}