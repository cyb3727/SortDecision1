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
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>

struct MemItems {
	size_t M;
	string M_unit;
	MemItems() {
	}
	MemItems(size_t M, string M_unit) {
		this->M = M;
		this->M_unit = M_unit;
	}

	map<string, double> dic_time;
	map<string, double> dic_speedup;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(M);
		ar& BOOST_SERIALIZATION_NVP(M_unit);
		ar& BOOST_SERIALIZATION_NVP(dic_time);
		ar& BOOST_SERIALIZATION_NVP(dic_speedup);
	}
};

class OutputXml {
private:
	friend class boost::serialization::access;
	size_t datavolume;
	vector<MemItems> v;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& BOOST_SERIALIZATION_NVP(datavolume);
		ar& BOOST_SERIALIZATION_NVP(v);
	}
public:
	OutputXml(size_t datavolume) {
		this->datavolume = datavolume;
	}

	void add(MemItems& a) {
		v.push_back(a);
	}
};