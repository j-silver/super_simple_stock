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
public:
	virtual pair<valarray<double>, valarray<double>> ts_seq() const = 0;

	virtual PricesParam const& get_params() const = 0;

	virtual double advance
		(double lP, double m, double d, double s, double e) = 0;

protected:
	virtual double* get_times_start() = 0;
	virtual double* get_prices_start() = 0;

public:
	friend void generate_prices(Prices& pricesObj, unsigned long);

	virtual double final_price() const = 0;

	virtual ~Prices() = default;
};


ostream& operator<<(ostream& os, const Prices& p)
{
	for (unsigned i {0}; i< p.ts_seq().first.size(); ++i)
		os << p.ts_seq().first[i] << "," << p.ts_seq().second[i]
			<< endl;
	return os;
}


class LinearPrices : public Prices {
	PricesParam pp;
	valarray<double> times;
	valarray<double> prices;
public:
	// constructor
	explicit LinearPrices(PricesParam param) :
		pp {param},
		times (pp.get_years()*pp.get_periods()),
		prices (pp.get_years()*pp.get_periods())
	{
		prices[0] = param.get_s0();
	};

	double advance(double lP, double m, double d, double s, double e) final
	{
		lP *= (1 + m*d + s*e*sqrt(d));
		return lP;
	};

	pair<valarray<double>, valarray<double>> ts_seq() const override
	{
		return {times, prices};
	}

protected:
	double* get_times_start() override { return &times[0]; }
	double* get_prices_start() override { return &prices[0]; }

public:
	PricesParam const& get_params() const override
	{
		return pp;
	}

	double final_price() const override
	{
		return *prev(end(prices));
	}

};


class LogPrices : public Prices {
	PricesParam pp;
	valarray<double> times;
	valarray<double> prices;
public:
	// constructor
	explicit LogPrices(PricesParam pp) :
		pp {pp},
		times (pp.get_years()*pp.get_periods()),
		prices (pp.get_years()*pp.get_periods())
	{
		prices[0] = log(pp.get_s0());
	};

	double advance(double lP, double m, double d, double s, double e) final
	{
		double nu {m - s*s/2};
		lP += (nu*d + s*e*sqrt(d));
		return lP;
	};

protected:
	double* get_times_start() override { return &times[0]; }
	double* get_prices_start() override { return &prices[0]; }

public:
	pair<valarray<double>, valarray<double>> ts_seq() const override
	{
		return {times, prices};
	}

	PricesParam const& get_params() const override
	{
		return pp;
	}

	double final_price() const override
	{
		return *prev(end(prices));
	}
};


void generate_prices(Prices& pricesObj, unsigned long seed)
{
	auto pp {pricesObj.get_params()};
	unsigned p {pp.get_periods()};
	double dt {1.0/p};
	unsigned long y {pp.get_years()};
	double mu  {pp.get_g().get_mu()};
	double sigma {pp.get_g().get_sigma()};
	double eps;

	normal_distribution<> normalDist;
	default_random_engine dre {seed};

	auto tIt {pricesObj.get_times_start()};
	auto pIt {pricesObj.get_prices_start()};
	double latestPrice {*pIt};
	for (unsigned long t {1}; t < y*p; ++t) {
		eps = normalDist(dre);
		*++tIt = static_cast<double>(t)*dt;
		latestPrice = pricesObj.advance(latestPrice, mu, dt, sigma, eps);
		*++pIt = latestPrice;
	}
}


map<string, double> scan_arguments(int narg, char* args[])
{
	string argumentsLine;
	for (int i {1}; i < narg; ++i)
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

	unsigned long seed {random_device{"/dev/random"}()};
	generate_prices(LP, seed);
	generate_prices(LogP, seed);

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
		//seed = random_device{"/dev/random"}();
		generate_prices(lp, seed);
		generate_prices(logp, seed);
		totFinalLin += lp.final_price();
		totFinalLog += logp.final_price();
	}
	cout << "Final Price Average: "
		<< log(totFinalLin/numberOfTries) << endl;
	cout << "Final LogPrice Aver: "
		<< totFinalLog/numberOfTries << endl;

	return 0;
}
