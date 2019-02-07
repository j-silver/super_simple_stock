// =============================================================================
//
//       Filename:  luenberger_ch13ex14.cpp
//
//    Description:  Exercise number 14, chapter 13 from Luenberger
//
//        Version:  1.0
//        Created:  06/04/17 19:37:27
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Giuseppe Argentieri (ga), gius.argentieri@gmail.com
//   Organization:
//
// =============================================================================

#include <iostream>
#include <fstream>
#include <random>
#include <valarray>
#include <map>
#include <regex>
using namespace std;

class GeomBrownMotion {
private:
	double Mu;			// mean value parameter
	double Sigma;		// variance parameter
public:
	// constructor
	GeomBrownMotion(double m, double s) : Mu {m}, Sigma {s}
	{
		if (m > 0 && s >= 0) ;
		else throw runtime_error("Bad values for GeomBrownMotion");
	}

	// default constructor
	GeomBrownMotion() : Mu {1}, Sigma {0} {};

	double mu() 	const	{ return Mu; };
	double sigma()	const	{ return Sigma; };
};


class PricesParam {
private:
	double S0;				// starting price
	unsigned long Years;
	unsigned Periods;		// number of periods per year
	GeomBrownMotion G;		// GBM parameters

public:
	// constructor (with some sanity checking)
	PricesParam(double s0, unsigned long y, unsigned p, GeomBrownMotion g) :
		S0 {s0}, Years {y}, Periods {p}, G {g}
	{
		if (s0>0 && y>0 && p>0) ;
		else throw runtime_error("Bad values for Prices");
	};

	double s0() const { return S0; };
	unsigned long years() const { return Years; };
	unsigned periods() const { return Periods; };
	const GeomBrownMotion& g() const { return G; };
};


class Prices {
protected:
	PricesParam PP;
	valarray<double> times;
	valarray<double> prices;

public:
	// constructor
	explicit Prices(PricesParam& ps) :
		PP {ps}, times {PP.years()*PP.periods()},
		prices {PP.years()*PP.periods()} {};

	const pair<valarray<double>, valarray<double>> TS_seq() const
		{ return {times, prices}; }

	virtual double advance
		(double l_p, double m, double D, double s, double e) = 0;
	virtual void GeneratePrices(long);
	double FinalPrice() const
	{
		const double* p = end(prices);
		return *--p;
	}

	virtual ~Prices() = default;
};


ostream& operator<<(ostream& os, const Prices& P)
{
	for (unsigned i {0}; i<P.TS_seq().first.size(); ++i)
		os << P.TS_seq().first[i] << "," << P.TS_seq().second[i]
			<< endl;
	return os;
}


class LinearPrices : public Prices {
public:
	// constructor
	explicit LinearPrices(PricesParam PP) : Prices(PP)
	{
		times[0] = 0;
		prices[0] = PP.s0();
	};

	double advance(double l_p, double m, double D, double s, double e) final
	{
		l_p *= (1 + m*D + s*e*sqrt(D));
		return l_p;
	};
};


class LogPrices : public Prices {
public:
	// constructor
	explicit LogPrices(PricesParam PP) : Prices(PP)
	{
		times[0] = 0;
		prices[0] = log(PP.s0());
	};

	double advance(double l_p, double m, double D, double s, double e) final
	{
		double nu {m - s*s/2};
		l_p += (nu*D + s*e*sqrt(D));
		return l_p;
	};
};


void Prices::GeneratePrices(long seed)
{
	unsigned p {PP.periods()};
	double DT {1.0/p};
	unsigned long y {PP.years()};
	double mu  {PP.g().mu()};
	double sigma {PP.g().sigma()};
	double eps;

	normal_distribution<> NormalDist;
	default_random_engine dre {seed};

	auto t_it {begin(times)};
	auto p_it {begin(prices)};
	double latest_price {*p_it};
	for (unsigned t{1}; t < y*p; ++t) {
		eps = NormalDist(dre);
		*++t_it = t*DT;
		latest_price = advance(latest_price, mu, DT, sigma, eps);
		*++p_it = latest_price;
	}
}


map<string, double> scan_arguments(int narg, char* args[])
{
	string arguments_line;
	for (int i{1}; i<narg; ++i)
		arguments_line += args[i];

	// default values
	constexpr double	T {1};
	constexpr double	S {1};
	constexpr int		p {12};
	constexpr double	mu {0.1};
	constexpr double	sigma {0.3};

	// pattern for the flags
	string T_string {R"(-T\s*(\d+))"};	// accepts only ints
	string S_string {R"(-S\s*(\d+(\.\d+)?))"};
	string p_string {R"(-p\s*(\d+))"};	// accepts only ints
	string mu_string {R"(-m\s*(\d+(\.\d+)?))"};
	string sigma_string {R"(-s\s*(\d+(\.\d+)?))"};

	vector<pair <string, double>> Matches {
		{T_string, T}, {S_string, S}, {p_string, p},
		{mu_string, mu}, {sigma_string, sigma}
	};

	smatch m;
	for (auto& pat : Matches) {
		if (regex_search(arguments_line, m, regex(pat.first)))
			pat.second = stod(m[1]);
	}

	map<string, double> params;
	params["T"] = {Matches[0].second};
	params["S"] = {Matches[1].second};
	params["p"] = {Matches[2].second};
	params["mu"] = {Matches[3].second};
	params["sigma"] = {Matches[4].second};

	return params;
}



int main(int argc, char* argv[])
{
	map<string, double> Parameters {scan_arguments(argc, argv)};

	double mu {Parameters["mu"]};
	double sigma {Parameters["sigma"]};
	GeomBrownMotion gbm {mu, sigma};

	double years {Parameters["T"]};
	double s0 {Parameters["S"]};
	double periods {Parameters["p"]};
	PricesParam pp {s0, static_cast<unsigned long>(years),
					static_cast<unsigned>(periods), gbm};

	LinearPrices	LP {pp};
	LogPrices	LogP {pp};

	long seed {time(nullptr)};
	LP.GeneratePrices(seed);
	LogP.GeneratePrices(seed);

	ofstream ofs {"linprices"};
	ofs << LP;
	ofs.close();
	ofs.open("logprices");
	ofs << LogP;

	constexpr unsigned int number_of_tries {1000};
	double tot_final_lin {0};
	double tot_final_log {0};
	for (unsigned n{0}; n < number_of_tries; ++n) {
		LinearPrices lp {pp};
		LogPrices  logp {pp};
		seed = time(nullptr);
		lp.GeneratePrices(seed);
		logp.GeneratePrices(seed);
		tot_final_lin += lp.FinalPrice();
		tot_final_log += logp.FinalPrice();
	}
	cout << "Final Price Average: "
		<< log(tot_final_lin/number_of_tries) << endl;
	cout << "Final LogPrice Aver: "
		<< tot_final_log/number_of_tries << endl;

	return 0;
}
