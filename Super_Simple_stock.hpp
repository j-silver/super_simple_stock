#include <utility>
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <chrono>
#include <string>


using Clock = std::chrono::high_resolution_clock;

class Stock {
	std::string _equity;
	unsigned _price;
	unsigned _lastDividend;
	double _fixDividend;	// It's a percentage
	unsigned _parValue;
	bool _common;
public:
	Stock(std::string eq,
		  unsigned pr,
		  unsigned ld = 0,
		  double fd = 0,
		  unsigned pv = 0,
		  bool c = true);

	[[nodiscard]] std::string get_equity() const { return _equity; }
	[[nodiscard]] unsigned get_price() const { return _price; }
	[[nodiscard]] unsigned get_last_dividend() const { return _lastDividend; }
	[[nodiscard]] double get_fix_div() const { return _fixDividend; }
	[[nodiscard]] bool is_common() const { return _common; }
	[[nodiscard]] unsigned get_par_value() const { return _parValue; }

	[[nodiscard]] double dividend_yield() const;
	[[nodiscard]] double pe_ratio() const;
};

enum class Type {Buy, Sell};

class Trade {

    Stock _stock;
	std::chrono::time_point<Clock> _timestamp;
	unsigned _quantity;
	Type _t;
	unsigned _tradePrice;
public:
	Trade(const Stock& s,
          const std::chrono::time_point<Clock>& ts,
          unsigned q,
          Type t,
          unsigned p)
		: _stock {s}, _timestamp {ts}, _quantity {q}, _t {t}, _tradePrice {p}
	{
		// basic check
		if (p == 0)
			throw std::domain_error("Price can't be zero.");
		if (s.get_equity().empty())
			throw std::domain_error("Stock's name empty.");
	}

	[[nodiscard]] std::string get_stock_name() const { return _stock.get_equity(); }
	[[nodiscard]] unsigned get_price() const { return _tradePrice; }
	[[nodiscard]] unsigned get_quantity() const { return _quantity; }
	[[nodiscard]] Type get_type() const { return _t; }
	[[nodiscard]] std::chrono::time_point<Clock> get_timestamp() const { return _timestamp; }
};


double
volume_weighted_stock_price(const std::vector<std::pair<double, unsigned>>& t);

// overload with a vector of trades as argument
double
volume_weighted_stock_price(const std::vector<Trade>& trades);


// Variadic template implementation
template<typename... Prices>
constexpr double geometric_mean(Prices... prices)
{
	double n {sizeof...(prices)};
	double geomMean {(std::pow(prices, 1.0 / n)* ...)}; // fold expr: requires C++17
	return geomMean;
}


double geometric_mean(const std::vector<double>& prices);


std::ostream& operator<<(std::ostream&, const std::vector<Trade>&);
std::ostream& operator<<(std::ostream& os, const std::vector<Stock>& stocks);
