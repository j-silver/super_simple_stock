#include "Super_Simple_stock.hpp"
#include <algorithm>
#include <iomanip>
#include <ctime>


double Stock::dividend_yield() const
{
	double divYld;
	if (_common)    // use the common formula
		divYld = static_cast<double>(_lastDividend) / static_cast<double>(_price);
	else // use the preferred formula
		divYld = _fixDividend /
                 100.0 *
                 static_cast<double>(_parValue) /
                 static_cast<double>(_price);

	return 100.0 * divYld; // It's a percentage
}


double Stock::pe_ratio() const
{
	double pe;

	if (_common) {
		if (_lastDividend == 0)
			throw std::domain_error("Can't calculate P/E: dividend is zero");
		else
			pe = static_cast<double>(_price) / static_cast<double>(_lastDividend);
	} else {
		if (_fixDividend == 0)
			throw std::domain_error("Can't calculate P/E: fixed dividend is zero");
		else
			pe = static_cast<double>(_price) / _fixDividend;
	}

	return pe;
}

Stock::Stock(std::string eq, unsigned pr, unsigned int ld, double fd,
             unsigned int pv, bool c)
		: _equity {std::move(eq)}, _price {pr}, _lastDividend {ld},
          _fixDividend {fd}, _parValue {pv}, _common {c}
{
	if (pr == 0 || (!c && pv == 0) || (!c && fd <= 0))
		throw std::domain_error("Invalid price or dividend or parValue");
}


// Geometric mean accepting a vector of prices as argument
double geometric_mean(const std::vector<double>& prices)
{
	if (std::any_of(prices.cbegin(), prices.cend(), [](double p) { return p <= 0; }))
		throw std::domain_error("Invalid prices");

	double n {static_cast<double>(prices.size())};
	double geomMean {
			std::accumulate(prices.cbegin(),
			                prices.cend(),
			                1.0,
			                [N = n](double p1, double p)
			                {
								return p1 *= std::pow(p, 1.0/N);
			                })
	};
	return geomMean;
}


// This implementation works with pairs (price, quantity)
double
volume_weighted_stock_price(const std::vector<std::pair<double, unsigned>>& trades)
{
	unsigned totalQuantity {
			std::accumulate(trades.cbegin(),
			                trades.cend(),
			                unsigned {0},
			                [](unsigned t, const auto& priceQuantity)
			                {
								return t += priceQuantity.second;
			                })
	};

	if (totalQuantity <= 0)
		throw std::domain_error("Total traded quantity must be positive");

	double numerator {
			std::accumulate(trades.cbegin(),
			                trades.cend(),
			                0.0,
			                [](double T, const auto& priceQuantity)
			                {
				                return T += static_cast<double>(priceQuantity.first) *
                                            static_cast<double>(priceQuantity.second);
			                })
	};

	return numerator / totalQuantity;
}


// overload with a vector of trades as argument
double volume_weighted_stock_price(const std::vector<Trade>& trades)
{
	auto now {Clock::now()};
	std::chrono::minutes last15Min {15};

	std::vector<std::pair<double, unsigned>> lastTrades;
	for (const auto& t : trades)
		if (now - t.get_timestamp() < last15Min)
			lastTrades.emplace_back(t.get_price(), t.get_quantity());

	double volWghtStkPrc {volume_weighted_stock_price(lastTrades)};

	return volWghtStkPrc;
}


std::ostream& operator<<(std::ostream& os, const std::vector<Trade>& trades)
{
	os.precision(6);
	os << std::string(62, '-') << std::endl;
	os << "Stock" << " | " << " Time " << std::string(17, ' ') << " | "
	   << "Quantity" << " | " << "Buy/Sell" << " | " << "Price" << std::endl;
	os << std::string(62, '-') << std::endl;
	os << std::setfill(' ');

	for (const auto& t : trades) {
		auto tradeTimestamp {Clock::to_time_t(t.get_timestamp())};
		std::string timestamp {(ctime(&tradeTimestamp))};
		timestamp.pop_back(); // remove the lf
		std::string time;
		if (Clock::now() - t.get_timestamp() < std::chrono::minutes {15})
			time = "*" + timestamp;
		else
			time = " " + timestamp;
		auto type(t.get_type() == Type::Buy ?
		          std::string {"Buy"} :
		          std::string {"Sell"});

		os << std::left << std::setw(6) << t.get_stock_name() << ' '
		   << std::setw(24) << time << "  " << std::right
		   << std::setw(7) << t.get_quantity() << std::string(4, ' ')
		   << std::setw(6) << type << std::string(4, ' ')
		   << std::setw(6) << t.get_price() << std::endl;
	}

	os << std::left;

	auto volWght {volume_weighted_stock_price(trades)};
	os << std::string(62, '-') << std::endl;
	os << "Volume Weighted Stock Price (*last 15 minutes): "
       << volWght << std::endl;

	return os;
}


std::ostream& operator<<(std::ostream& os, const std::vector<Stock>& stocks)
{
	os.precision(6);
	std::string line(78, '-');
	os << line << std::endl;
	os << "Stock" << " | " << "Type" << " | " << "Last Div" << " | "
	   << "Fix Div(%)" << " | " << "Par Val" << " | "
	   << "Yield(%)" << " | " << "P/E Ratio" << " | " << "Price\n";
	os << line << std::endl;

	for (const auto& s : stocks) {
		std::string Type {s.is_common() ? "common" : "prefer"};

		os << std::left << std::setw(6) << s.get_equity() << ' '
		   << std::setw(6) << Type << ' ';
		os.precision(2);
		os << std::right;
		os << std::setw(6) << s.get_last_dividend() << std::string(5, ' ')
		   << std::setw(7) << s.get_fix_div()       << std::string(6, ' ')
		   << std::setw(6) << s.get_par_value()     << std::string(5, ' ')
		   << std::fixed
		   << std::setw(6) << s.dividend_yield()    << std::string(5, ' ')
		   << std::setw(8) << s.pe_ratio()          << std::string(3, ' ')
		   << std::defaultfloat
		   << std::setw(6) << s.get_price() << std::endl;
	}
	os << line << std::endl;

	std::vector<double> prices;
	prices.reserve(stocks.size());

	for (const auto& s : stocks)
		prices.push_back(s.get_price());

	os.precision(6);
	os << "GBCE All Share Index: " << geometric_mean(prices) << std::endl;

	return os;
}
