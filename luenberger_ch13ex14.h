#ifndef LUENBERGER_CH13EX14_H
#define LUENBERGER_CH13EX14_H

#include <iostream>
#include <fstream>
#include <random>
#include <valarray>
#include <map>
#include <regex>


enum class Integration { Linear, Logarithmic };

using oneStepAdvance = double (*)(double lP, double m, double d, double s, double e);

double linear_advance(double lP, double m, double d, double s, double e);
double logarithmic_advance(double lP, double m, double d, double s, double e);


class GeomBrownMotion {
private:
	double mu;			// mean value parameter
	double sigma;		// variance parameter
public:
	// constructor
	GeomBrownMotion(double m, double s);

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
	PricesParam(double s0, unsigned long y, unsigned p, GeomBrownMotion g);

	double get_s0() const { return s0; };
	unsigned long get_years() const { return years; };
	unsigned get_periods() const { return periods; };
	const GeomBrownMotion& get_g() const { return g; };
};


class Prices {
	PricesParam pp;
	std::valarray<double> times;
	std::valarray<double> prices;
	oneStepAdvance advance;
public:
	// constructor
	explicit Prices(PricesParam param, Integration type);

	std::pair<std::valarray<double>, std::valarray<double>> ts_seq() const
	{
		return {times, prices};
	}

	void generate_prices(unsigned long seed);

	double final_price() const
	{
		return *std::prev(end(prices));
	}
};


std::ostream& operator<<(std::ostream& os, const Prices& p);

#endif // LUENBERGER_CH13EX14_H
