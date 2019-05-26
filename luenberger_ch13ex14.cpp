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

#include "luenberger_ch13ex14.h"


double linear_advance(double lP, double m, double d, double s, double e)
{
	lP *= (1 + m*d + s*e*sqrt(d));
	return lP;
}


double logarithmic_advance(double lP, double m, double d, double s, double e)
{
	double nu {m - s*s/2};
	lP += (nu*d + s*e*sqrt(d));
	return lP;
}



GeomBrownMotion::GeomBrownMotion(double m, double s) : mu {m}, sigma {s}
{
	if (m > 0 && s >= 0) ;
	else throw std::runtime_error("Bad values for GeomBrownMotion");
}


PricesParam::PricesParam(double s0, unsigned long y, unsigned p, GeomBrownMotion g) :
		s0 {s0}, years {y}, periods {p}, g {g}
{
	if (s0>0 && y>0 && p>0) ;
	else throw std::runtime_error("Bad values for Prices");
}


std::ostream& operator<<(std::ostream& os, const Prices& p)
{
	for (unsigned i {0}; i< p.ts_seq().first.size(); ++i)
		os << p.ts_seq().first[i] << "," << p.ts_seq().second[i]
		   << '\n';
	return os;
}



void Prices::generate_prices(unsigned long seed)
{
	unsigned p {pp.get_periods()};
	double dt {1.0/p};
	unsigned long y {pp.get_years()};
	double mu  {pp.get_g().get_mu()};
	double sigma {pp.get_g().get_sigma()};
	double eps;

	std::normal_distribution<> normalDist;
	std::default_random_engine dre {seed};

	auto tIt {&times[0]};
	auto pIt {&prices[0]};
	double latestPrice {*pIt};
	for (unsigned long t {1}; t < y*p; ++t) {
		eps = normalDist(dre);
		*++tIt = static_cast<double>(t)*dt;
		latestPrice = advance(latestPrice, mu, dt, sigma, eps);
		*++pIt = latestPrice;
	}
}

Prices::Prices(PricesParam param, Integration type) :
		pp {param},
		times (pp.get_years()*pp.get_periods()),
		prices (pp.get_years()*pp.get_periods()),
		advance {integrationType[type]}
{
	prices[0] = param.get_s0();
}


std::map<std::string, double> scan_arguments(int narg, char* args[])
{
	std::string argumentsLine;
	for (int i {1}; i < narg; ++i)
		argumentsLine += args[i];

	// default values
	constexpr double	T {1};
	constexpr double	S {1};
	constexpr int		p {12};
	constexpr double	mu {0.1};
	constexpr double	sigma {0.3};

	// pattern for the flags
	std::string tString {R"(-T\s*(\d+))"};	// accepts only ints
	std::string sString {R"(-S\s*(\d+(\.\d+)?))"};
	std::string pString {R"(-p\s*(\d+))"};	// accepts only ints
	std::string muString {R"(-m\s*(\d+(\.\d+)?))"};
	std::string sigmaString {R"(-s\s*(\d+(\.\d+)?))"};

	std::vector<std::pair <std::string, double>> matches {
		{tString, T}, {sString, S}, {pString, p},
		{muString, mu}, {sigmaString, sigma}
	};

	std::smatch m;
	for (auto& pat : matches) {
		if (regex_search(argumentsLine, m, std::regex(pat.first)))
			pat.second = stod(m[1]);
	}

	std::map<std::string, double> params;
	params["T"] = {matches[0].second};
	params["S"] = {matches[1].second};
	params["p"] = {matches[2].second};
	params["mu"] = {matches[3].second};
	params["sigma"] = {matches[4].second};

	return params;
}



int main(int argc, char* argv[])
{
	std::map<std::string, double> parameters {scan_arguments(argc, argv)};

	double mu {parameters["mu"]};
	double sigma {parameters["sigma"]};
	GeomBrownMotion gbm {mu, sigma};

	double years {parameters["T"]};
	double s0 {parameters["S"]};
	double periods {parameters["p"]};
	PricesParam pp {s0, static_cast<unsigned long>(years),
					static_cast<unsigned>(periods), gbm};

	Prices	LP      {pp, Integration::Linear};
	Prices	LogP    {pp, Integration::Logarithmic};

	unsigned long seed {std::random_device{"/dev/random"}()};
	LP.generate_prices(seed);
	LogP.generate_prices(seed);

	std::ofstream ofs {"linprices"};
	ofs << LP;
	ofs.close();
	ofs.open("logprices");
	ofs << LogP;

	constexpr unsigned int numberOfTries {1000};
	double totFinalLin {0};
	double totFinalLog {0};
	for (unsigned n{0}; n < numberOfTries; ++n) {
		Prices lp   {pp, Integration::Linear};
		Prices logp {pp, Integration::Logarithmic};
		lp.generate_prices(seed);
		logp.generate_prices(seed);
		totFinalLin += lp.final_price();
		totFinalLog += logp.final_price();
	}
	std::cout << "Final Price Average: "
		<< log(totFinalLin/numberOfTries) << '\n';
	std::cout << "Final LogPrice Aver: "
		<< totFinalLog/numberOfTries << '\n';

	return 0;
}
