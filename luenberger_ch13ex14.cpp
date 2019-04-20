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
	double mu;			// mean value parameter
	double sigma;		// variance parameter
public:
	// constructor
	GeomBrownMotion(double m, double s) : mu {m}, sigma {s}
	{
		if (m > 0 && s >= 0) ;
		else throw runtime_error("Bad values for GeomBrownMotion");
	}

	// default constructor
	GeomBrownMotion() : mu {1}, sigma {0} {};

	double get_mu() 	const	{ return mu; };
	double get_sigma()	const	{ return sigma; };
};


class PricesParam {
private:
	double s0;				// starting price
	unsigned long years;
	unsigned periods;		// number of periods per year
	GeomBrownMotion g;		// GBM parameters

public:
	// constructor (with some sanity checking)
	PricesParam(double s0, unsigned long y, unsigned p, GeomBrownMotion g) :
		s0 {s0}, years {y}, periods {p}, g {g}
	{
		if (s0>0 && y>0 && p>0) ;
		else throw runtime_error("Bad values for Prices");
	};

	double get_s0() const { return s0; };
	unsigned long get_years() const { return years; };
	unsigned get_periods() const { return periods; };
	const GeomBrownMotion& get_g() const { return g; };
};


class Prices {
protected:
	PricesParam pp;
	valarray<double> times;
	valarray<double> prices;

public:
	// constructor
	explicit Prices(PricesParam& ps) :
		pp {ps}, times (pp.get_years()*pp.get_periods()),
		prices (pp.get_years()*pp.get_periods()) {};

	const pair<valarray<double>, valarray<double>> TS_seq() const
		{ return {times, prices}; }

	virtual double advance
		(double lP, double m, double d, double s, double e) = 0;
	virtual void generate_prices(long);
	double final_price() const
	{
		return *prev(end(prices));
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
		prices[0] = PP.get_s0();
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
	explicit LogPrices(PricesParam pp) : Prices(pp)
	{
		times[0] = 0;
		prices[0] = log(pp.get_s0());
	};

	double advance(double lP, double m, double d, double s, double e) final
	{
		double nu {m - s*s/2};
		lP += (nu*d + s*e*sqrt(d));
		return lP;
	};
};


void Prices::generate_prices(long seed)
{
	unsigned p {pp.get_periods()};
	double dt {1.0/p};
	unsigned long y {pp.get_years()};
	double mu  {pp.get_g().get_mu()};
	double sigma {pp.get_g().get_sigma()};
	double eps;

	normal_distribution<> normalDist;
	default_random_engine dre {seed};

	auto tIt {begin(times)};
	auto pIt {begin(prices)};
	double latestPrice {*pIt};
	for (unsigned t{1}; t < y*p; ++t) {
		eps = normalDist(dre);
		*++tIt = t*dt;
		latestPrice = advance(latestPrice, mu, dt, sigma, eps);
		*++pIt = latestPrice;
	}
}


map<string, double> scan_arguments(int narg, char* args[])
{
	string argumentsLine;
	for (int i{1}; i<narg; ++i)
		argumentsLine += args[i];

	// default values
	constexpr double	T {1};
	constexpr double	S {1};
	constexpr int		p {12};
	constexpr double	mu {0.1};
	constexpr double	sigma {0.3};

	// pattern for the flags
	string tString {R"(-T\s*(\d+))"};	// accepts only ints
	string sString {R"(-S\s*(\d+(\.\d+)?))"};
	string pString {R"(-p\s*(\d+))"};	// accepts only ints
	string muString {R"(-m\s*(\d+(\.\d+)?))"};
	string sigmaString {R"(-s\s*(\d+(\.\d+)?))"};

	vector<pair <string, double>> matches {
		{tString, T}, {sString, S}, {pString, p},
		{muString, mu}, {sigmaString, sigma}
	};

	smatch m;
	for (auto& pat : matches) {
		if (regex_search(argumentsLine, m, regex(pat.first)))
			pat.second = stod(m[1]);
	}

	map<string, double> params;
	params["T"] = {matches[0].second};
	params["S"] = {matches[1].second};
	params["p"] = {matches[2].second};
	params["mu"] = {matches[3].second};
	params["sigma"] = {matches[4].second};

	return params;
}



int main(int argc, char* argv[])
{
	map<string, double> parameters {scan_arguments(argc, argv)};

	double mu {parameters["mu"]};
	double sigma {parameters["sigma"]};
	GeomBrownMotion gbm {mu, sigma};

	double years {parameters["T"]};
	double s0 {parameters["S"]};
	double periods {parameters["p"]};
	PricesParam pp {s0, static_cast<unsigned long>(years),
					static_cast<unsigned>(periods), gbm};

	LinearPrices	LP {pp};
	LogPrices	LogP {pp};

	long seed {time(nullptr)};
	LP.generate_prices(seed);
	LogP.generate_prices(seed);

	ofstream ofs {"linprices"};
	ofs << LP;
	ofs.close();
	ofs.open("logprices");
	ofs << LogP;

	constexpr unsigned int numberOfTries {1000};
	double totFinalLin {0};
	double totFinalLog {0};
	for (unsigned n{0}; n < numberOfTries; ++n) {
		LinearPrices lp {pp};
		LogPrices  logp {pp};
		seed = time(nullptr);
		lp.generate_prices(seed);
		logp.generate_prices(seed);
		totFinalLin += lp.final_price();
		totFinalLog += logp.final_price();
	}
	cout << "Final Price Average: "
		<< log(totFinalLin/numberOfTries) << endl;
	cout << "Final LogPrice Aver: "
		<< totFinalLog/numberOfTries << endl;

	return 0;
}
