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


std::map<Integration, oneStepAdvance> integrationType {
		{Integration::Linear, linear_advance},
		{Integration::Logarithmic, logarithmic_advance}
};


double linear_advance(double lP, double m, double d, double s, double e)
{
	lP *= (1 + m*d + s*e*sqrt(d));
	return lP;
}


double logarithmic_advance(double lP, double m, double d, double s, double e)
{
	double nu {m - s*s/2};
	lP *= (1 + nu*d + s*e*sqrt(d));
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
	if (s0 > 0 && y > 0 && p > 0) ;
	else throw std::runtime_error("Bad values for Prices");
}


Prices::Prices(PricesParam param, Integration type) :
		pp {param},
		times (pp.get_years()*pp.get_periods()),
		prices (pp.get_years()*pp.get_periods()),
		advance {integrationType[type]}
{
	prices[0] = param.get_s0();
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


std::ostream& operator<<(std::ostream& os, const Prices& p)
{
	for (unsigned i {0}; i < p.ts_seq().first.size(); ++i)
		os << p.ts_seq().first[i] << "," << p.ts_seq().second[i]
		   << '\n';
	return os;
}


