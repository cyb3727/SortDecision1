#pragma once
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <random>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/time.h>
#include <boost/archive/xml_oarchive.hpp> 
#include <boost/archive/xml_iarchive.hpp> 
#include <boost/serialization/vector.hpp>
#include "convex.h"
using namespace std;

const double eps = 1.0e-6;
const size_t one_month = 30 * 24 * 3600;

struct Point {
	size_t rw_unit;
	double bandwidth;

	bool operator<(const Point& b) const
	{
		return this->rw_unit < b.rw_unit;
	}

	friend std::ostream& operator << (std::ostream& out, Point& d)
	{
		std::cout << "{" << d.rw_unit << "," << d.bandwidth << "}";
	}

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(rw_unit);
		ar& BOOST_SERIALIZATION_NVP(bandwidth);
	}
};

class LinearFunction {
private:
	friend class boost::serialization::access;

	vector<Point> P;
	vector<Point> Cov;
	vector<Point> FCov;
	size_t seconds;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version);

	void Copy(vector<Point>* origin, vector<Point>* copy);
	void Convex(vector<Point>* P, vector<Point>* Cov);
	void FilterConvex(vector<Point>* Cov, vector<Point>* FCov);
public:
	LinearFunction() {}
	LinearFunction(vector<Point>* Ptr);

	double f(double U);
	size_t getsec();
};

inline LinearFunction::LinearFunction(vector<Point>* Ptr) {
	Copy(Ptr, &P);
	Convex(&P, &Cov);
	FilterConvex(&Cov, &FCov);
	struct timeval time;
	gettimeofday(&time, NULL);
	seconds = time.tv_sec;
}

inline double LinearFunction::f(double U)
{
	if (U <= FCov[0].rw_unit)
	{
		return FCov[0].bandwidth;
	}

	size_t size = FCov.size();
	for (size_t i = 0; i < size; i++)
	{
		if (FCov[i].rw_unit > U) {
			return (U - FCov[i - 1].rw_unit) * (FCov[i].bandwidth - FCov[i - 1].bandwidth) / (FCov[i].rw_unit - FCov[i - 1].rw_unit) + FCov[i - 1].bandwidth;
		}
	}
	return FCov[size - 1].bandwidth;
}

inline size_t LinearFunction::getsec()
{
	return seconds;
}

template<typename Archive>
inline void LinearFunction::serialize(Archive& ar, const unsigned int version)
{
	ar& BOOST_SERIALIZATION_NVP(P);
	ar& BOOST_SERIALIZATION_NVP(Cov);
	ar& BOOST_SERIALIZATION_NVP(FCov);
	ar& BOOST_SERIALIZATION_NVP(seconds);
}

inline void LinearFunction::Copy(vector<Point>* origin, vector<Point>* copy) {
	size_t size = origin->size();
	for (size_t i = 0; i < size; i++)
	{
		copy->push_back(origin->at(i));
	}
}

inline void LinearFunction::Convex(vector<Point>* P, vector<Point>* Cov) {
	size_t size = P->size();
	point* p = new point[size];
	point* res = new point[size];

	for (size_t i = 0; i < size; i++)
	{
		p[i].x = (*P)[i].rw_unit;
		p[i].y = (*P)[i].bandwidth;
	}

	size_t res_size = graham(p, size, res);
	for (size_t i = 0; i < res_size; i++)
	{
		Point tmp;
		tmp.rw_unit = res[i].x;
		tmp.bandwidth = res[i].y;
		Cov->push_back(tmp);
	}

	delete res;
	delete p;
}

inline void LinearFunction::FilterConvex(vector<Point>* Cov, vector<Point>* FCov) {
	//fetch the minimal unit and maximal bandwidth convex point
	size_t min_index = -1;
	size_t max_index = -1;
	size_t size = Cov->size();

	size_t min_unit;
	size_t max_bandwidth;

	for (size_t i = 0; i < size; i++)
	{
		if (min_index < 0)
		{
			min_index = i;
			min_unit = Cov->at(i).rw_unit;
		}
		else
		{
			if (Cov->at(i).rw_unit < min_unit) {
				min_unit = Cov->at(i).rw_unit;
				min_index = i;
			}
		}

		if (max_index < 0)
		{
			max_index = i;
			max_bandwidth = Cov->at(i).bandwidth;
		}
		else
		{
			if (Cov->at(i).bandwidth > max_bandwidth)
			{
				max_bandwidth = Cov->at(i).bandwidth;
				max_index = i;
			}
		}
	}

	//fetch all the points above the line from the min_index point to the max_index point
	for (size_t i = 0; i < size; i++)
	{
		double cmp_value = Cov->at(i).bandwidth - (Cov->at(i).rw_unit - Cov->at(min_index).rw_unit) * (Cov->at(max_index).bandwidth - Cov->at(min_index).bandwidth) / (Cov->at(max_index).rw_unit - Cov->at(min_index).rw_unit) - Cov->at(min_index).bandwidth;
		if (cmp_value > 0 || abs(cmp_value) < eps) {
			FCov->push_back(Cov->at(i));
		}
	}

	//sort the FCov in ascending order
	sort(FCov->begin(), FCov->end());
}
