#include "Super_Simple_stock.hpp"
#include <algorithm>
#include <iomanip>
#include <ctime>


double Stock::dividend_yield() const
{
	double div_yld {0.0};
	if (common)    // use the common formula
		div_yld = static_cast<double>(last_dividend)/static_cast<double>(price);
	else // use the preferred formula
		div_yld = fix_dividend/
		          100.0*
		          static_cast<double>(par_value)/
		          static_cast<double>(price);

	return 100.0*div_yld; // It's a percentage
}


double Stock::pe_ratio() const
{
	double pe {0.0};

	if (common) {
		if (last_dividend == 0)
			throw std::domain_error("Can't calculate P/E: dividend is zero");
		else
			pe = static_cast<double>(price)/static_cast<double>(last_dividend);
	} else {
		if (fix_dividend == 0)
			throw std::domain_error("Can't calculate P/E: fixed dividend is zero");
		else
			pe = static_cast<double>(price)/fix_dividend;
	}

	return pe;
}

Stock::Stock(std::string eq, unsigned pr, unsigned int ld, double fd,
             unsigned int pv, bool c)
		: Equity {std::move(eq)}, price {pr}, last_dividend {ld},
		  fix_dividend {fd}, par_value {pv}, common {c}
{
	if (pr == 0 || (!c && pv == 0) || (!c && fd <= 0))
		throw std::domain_error("Invalid price or dividend or par_value");
}


// Geometric mean accepting a vector of prices as argument
double geometric_mean(const std::vector<double>& prices)
{
	if (std::any_of(prices.cbegin(), prices.cend(), [](double p) { return p <= 0; }))
		throw std::domain_error("Invalid prices");

	double n {static_cast<double>(prices.size())};
	double geom_mean {
			std::accumulate(prices.cbegin(),
			                prices.cend(),
			                1.0,
			                [N = n](double p1, double p)
			                {
								return p1 *= std::pow(p, 1.0/N);
			                })
	};
	return geom_mean;
}


// This implementation works with pairs (price, quantity)
double
volume_weighted_stock_price(const std::vector<std::pair<double, unsigned>>& trades)
{
	unsigned total_quantity {
			std::accumulate(trades.cbegin(),
			                trades.cend(),
			                unsigned {0},
			                [](unsigned T, const auto& price_quantity)
			                {
								return T += price_quantity.second;
			                })
	};

	if (total_quantity <= 0)
		throw std::domain_error("Total traded quantity must be positive");

	double numerator {
			std::accumulate(trades.cbegin(),
			                trades.cend(),
			                0.0,
			                [](double T, const auto& price_quantity)
			                {
				                return T += static_cast<double>(price_quantity.first)*
				                            static_cast<double>(price_quantity.second);
			                })
	};

	return numerator/total_quantity;
}


// overload with a vector of trades as argument
double volume_weighted_stock_price(const std::vector<Trade>& trades)
{
	auto now {Clock::now()};
	std::chrono::minutes last_15min {15};

	std::vector<std::pair<double, unsigned>> last_trades;
	for (const auto& t : trades)
		if (now - t.get_timestamp() < last_15min)
			last_trades.emplace_back(t.get_price(), t.get_quantity());

	double vol_wght_stk_prc {volume_weighted_stock_price(last_trades)};

	return vol_wght_stk_prc;
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
		auto trade_timestamp {Clock::to_time_t(t.get_timestamp())};
		std::string time {(ctime(&trade_timestamp))};
		time.pop_back(); // remove the lf
		if (Clock::now() - t.get_timestamp() < std::chrono::minutes {15})
			time = "*" + time;
		else
			time = " " + time;
		auto type(t.get_type() == type::buy ?
		          std::string {"Buy"} :
		          std::string {"Sell"});

		os << std::left << std::setw(6) << t.get_stock_name() << ' '
		   << std::setw(24) << time << "  " << std::right
		   << std::setw(7) << t.get_quantity() << std::string(4, ' ')
		   << std::setw(6) << type << std::string(4, ' ')
		   << std::setw(6) << t.get_price() << std::endl;
	}

	os << std::left;

	auto VOL_WGHT {volume_weighted_stock_price(trades)};
	os << std::string(62, '-') << std::endl;
	os << "Volume Weighted Stock Price (*last 15 minutes): "
	   << VOL_WGHT << std::endl;

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
