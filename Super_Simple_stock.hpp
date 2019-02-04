#include <utility>
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <chrono>
#include <string>


using Clock = std::chrono::high_resolution_clock;

class Stock {
	std::string Equity;
	unsigned price;
	unsigned last_dividend;
	double fix_dividend;	// It's a percentage
	unsigned par_value;
	bool common;
public:
	Stock(std::string eq,
		  unsigned pr,
		  unsigned ld = 0,
		  double fd = 0,
		  unsigned pv = 0,
		  bool c = true);

	std::string get_equity() const { return Equity; }
	unsigned get_price() const { return price; }
	unsigned get_last_dividend() const { return last_dividend; }
	double get_fix_div() const { return fix_dividend; }
	bool is_common() const { return common; }
	unsigned get_par_value() const { return par_value; }

	double dividend_yield() const;
	double pe_ratio() const;
};


enum class type {buy, sell};

class Trade {
	Stock stock;
	std::chrono::time_point<Clock> timestamp;
	unsigned quantity;
	type t;
	unsigned trade_price;
public:
	Trade(const Stock& s,
		  const std::chrono::time_point<Clock>& ts,
		  unsigned q,
		  type T,
		  unsigned p)
		: stock {s}, timestamp {ts}, quantity {q}, t {T}, trade_price {p}
	{
		// basic check
		if (p == 0)
			throw std::domain_error("Price can't be zero.");
		if (s.get_equity().empty())
			throw std::domain_error("Stock's name empty.");
	}

	std::string get_stock_name() const { return stock.get_equity(); }
	unsigned get_price() const { return trade_price; }
	unsigned get_quantity() const { return quantity; }
	type get_type() const { return t; }
	std::chrono::time_point<Clock> get_timestamp() const { return timestamp; }
};


double
volume_weighted_stock_price(const std::vector<std::pair<double, unsigned>>& trades);

// overload with a vector of trades as argument
double
volume_weighted_stock_price(const std::vector<Trade>& trades);


// Variadic template implementation
template<typename... Prices>
constexpr double geometric_mean(Prices... prices)
{
	double n {sizeof...(prices)};
	double geom_mean {(std::pow(prices, 1.0/n)* ...)}; // fold expr: requires C++17
	return geom_mean;
}


double geometric_mean(const std::vector<double>& prices);


std::ostream& operator<<(std::ostream&, const std::vector<Trade>&);
std::ostream& operator<<(std::ostream& os, const std::vector<Stock>& stocks);
