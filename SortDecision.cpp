#include "SortDecision.h"

void SortDecision::save(LinearFunction& LFr, LinearFunction& LFw)
{
	std::ofstream ss(this->decision_file_path);
	boost::archive::xml_oarchive oa(ss);
	oa << BOOST_SERIALIZATION_NVP(LFr) << BOOST_SERIALIZATION_NVP(LFw);
}

void SortDecision::load(LinearFunction& LFr, LinearFunction& LFw)
{
	std::ifstream ss(this->decision_file_path);
	boost::archive::xml_iarchive ia(ss);
	ia >> BOOST_SERIALIZATION_NVP(LFr) >> BOOST_SERIALIZATION_NVP(LFw);
}

LinearFunction* SortDecision::CalcFw(char* file_path, size_t M, size_t B)
{
	UnitTest u;
	size_t U = B;
	vector<Point> P;
	while (U < M)
	{
		double bandwidth;
		Point tmp;
		u.TestBlockManagerRandWriteSpeed(U, file_path, bandwidth, 10);
		tmp.rw_unit = U;
		tmp.bandwidth = bandwidth;
		P.push_back(tmp);
		U = 2 * U;
	}
	LinearFunction* Fw = new LinearFunction(&P);
	return Fw;
}

LinearFunction* SortDecision::CalcFr(char* file_path, size_t M, size_t B)
{
	UnitTest u;
	size_t U = B;
	vector<Point> P;
	while (U < M)
	{
		double bandwidth;
		Point tmp;
		u.TestBlockManagerRandReadSpeed(U, file_path, bandwidth, 10);
		tmp.rw_unit = U;
		tmp.bandwidth = bandwidth;
		P.push_back(tmp);
		U = 2 * U;
	}
	LinearFunction* Fr = new LinearFunction(&P);
	return Fr;
}

tuple<double, size_t, size_t> SortDecision::Trisection(size_t N, size_t M, size_t B, size_t k, LinearFunction* Fr, LinearFunction* Fw)
{
	double L = B;
	double R = (M - B) / k;
	while (L + eps < R)
	{
		double tri = (R - L) / 3;
		double m1 = L + tri;
		double m2 = m1 + tri;
		double c1 = TCost(N, M, m1, k, Fr, Fw);
		double c2 = TCost(N, M, m2, k, Fr, Fw);
		//printf("range: (%.2lf, %.2lf) pos: (%.2lf, %.2lf) value: (%.2lf,%.2lf)\n", L, R, m1, m2, c1, c2);
		if (c1 <= c2) {
			R = m2;
		}
		else
		{
			L = m1;
		}
	}

	size_t low_L = floor(L / B) * B;
	size_t up_L = ceil(L / B) * B;
	if (M <= k * up_L) {
		double T_low = TCost(N, M, low_L, k, Fr, Fw);
		return make_tuple(T_low, low_L, M - k * low_L);
	}
	else {
		double T_low = TCost(N, M, low_L, k, Fr, Fw);
		double T_up = TCost(N, M, up_L, k, Fr, Fw);
		if (T_low < T_up)
		{
			return make_tuple(T_low, low_L, M - k * low_L);
		}
		else {
			return make_tuple(T_up, up_L, M - k * up_L);
		}
	}
}

double SortDecision::TCost(size_t N, size_t M, double Br, size_t k, LinearFunction* Fr, LinearFunction* Fw)
{
	double Volume = N * ceil(log(((N + M - 1) / M)) / log(k)) / (1024ULL * 1024);
	double T = 1.0 * N / (1024ULL * 1024) / Fr->f(M) + Volume / Fr->f(Br) + 1.0 * N / (1024ULL * 1024) / Fw->f(M) + Volume / Fw->f(M - k * Br);
	/*size_t x = 1024ULL * 4;
	while (x < 1024ULL * 1024 * 1) {
		printf("f: %.2lf\n", Fr->f(x));
		x = 2 * x;
	}*/
	return T;
}

double SortDecision::NormalCost(size_t N, size_t M, size_t B, size_t output_size, LinearFunction* Fr, LinearFunction* Fw)
{
	size_t O = output_size;
	size_t k = min(N / M + ((N % M) > 0), (M - O) / B);
	//size_t U = max((M / k) / B * B, B);
	//size_t O = max(M - k * U, B);
	size_t U = max((M - O) / k / B * B, B);

	//double T = TCost(N, M, U, k, Fr, Fw);
	double Volume = N * ceil(log(((N + M - 1) / M)) / log(k)) / (1024ULL * 1024);
	double T = N / (1024ULL * 1024) / Fr->f(M) + N / (1024ULL * 1024) / Fw->f(M) + Volume * (1 / Fr->f(U) + (1 / Fw->f(O)));
	return T;
}

double SortDecision::TraditionalCost(size_t N, size_t M, size_t B, LinearFunction* Fr, LinearFunction* Fw)
{
	size_t k = min(N / M + ((N % M) > 0), M / B - 1);
	size_t U = M / (k + 1);
	double Volume = N * ceil(log(((N + M - 1) / M)) / log(k)) / (1024ULL * 1024);
	double T = N / (1024ULL * 1024) / Fr->f(M) + N / (1024ULL * 1024) / Fw->f(M) + Volume * (1 / Fr->f(U) + (1 / Fw->f(U)));
	return T;
}

bool SortDecision::exist(char* path)
{
	return (access(path, F_OK) != -1);
}

size_t SortDecision::myceil(double x)
{
	size_t low = floor(x);
	size_t up = ceil(x);
	if (x < (low + eps)) {
		return low;
	}
	else
	{
		return up;
	}
}

int SortDecision::createfile(char* file_path, size_t size)
{
	size_t count = size / (1024ULL * 1024);
	ostringstream s;
	s << "dd if=/dev/zero of=";
	s << file_path;
	s << " bs=1M count=";
	s << count;
	std::cout << s.str() << std::endl;
	return system(s.str().c_str());
}

int SortDecision::removefile(char* file_path)
{
	ostringstream s;
	s << "rm -rf ";
	s << file_path;
	std::cout << s.str() << std::endl;
	return system(s.str().c_str());
}

void SortDecision::GetOnlyArchive(size_t M_max, size_t B)
{
	int res = createfile(test_file_path, M_max);

	LinearFunction* fr, * fw;
	fr = CalcFr(test_file_path, M_max, B);
	fw = CalcFw(test_file_path, M_max, B);
	save(*fr, *fw);
}

SortDecision::SortDecision(const char* decision_file_path, const char* test_file_path)
{
	strcpy(this->decision_file_path, decision_file_path);
	strcpy(this->test_file_path, test_file_path);
}

tuple<double, size_t, size_t, size_t> SortDecision::Decide(size_t N, size_t M, size_t M_max, size_t B)
{
	size_t kmax = min(N / M + (N % M > 0), M / B - 1);
	double T_min = -1;//-1 means infity because of the size_t is unsigned type

	LinearFunction Fr, Fw;
	LinearFunction* fr, * fw;
	bool re_calculate = false;
	if (exist(this->decision_file_path))
	{
		load(Fr, Fw);
		timeval now;
		gettimeofday(&now, NULL);
		if (now.tv_sec - Fr.getsec() >= one_month) {
			removefile(this->decision_file_path);
			re_calculate = true;
		}
		else
		{
			fr = &Fr;
			fw = &Fw;
		}
	}
	else
	{
		re_calculate = true;
	}

	if (re_calculate == true) {
		//create a file size of M_max
		int res = createfile(test_file_path, M_max);
		if (res == 0) {
			fr = CalcFr(test_file_path, M_max, B);
			fw = CalcFw(test_file_path, M_max, B);
			save(*fr, *fw);
		}
		else
		{
			printf("create file finished!\n");
			return make_tuple(-1, 0, 0, 0);
		}
	}

	tuple<double, size_t, size_t, size_t> res;
	for (size_t k = 2; k <= kmax; k++)
	{
		tuple<double, size_t, size_t> tmp = Trisection(N, M, B, k, fr, fw);
		double T1 = get<0>(tmp);
		size_t Br = get<1>(tmp);
		size_t Bw = get<2>(tmp);
		if (T1 < T_min || T_min < 0)
		{
			T_min = T1;
			res = make_tuple(T_min, k, Br, Bw);
		}
	}
	return res;
}

double SortDecision::RetNormalCost(size_t N, size_t M, size_t B, size_t output_size)
{
	LinearFunction Fr, Fw;
	load(Fr, Fw);
	return NormalCost(N, M, B, output_size, &Fr, &Fw);
}

double SortDecision::RetTraditionalCost(size_t N, size_t M, size_t B)
{
	LinearFunction Fr, Fw;
	load(Fr, Fw);
	return TraditionalCost(N, M, B, &Fr, &Fw);
}
