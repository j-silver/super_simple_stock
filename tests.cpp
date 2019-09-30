#include "Super_Simple_stock.hpp"
#include <random>

// constants for random generation of stocks, prices, dividends, etc.
constexpr unsigned N {30};	// number of stocks
constexpr int min_price {5};	// min market price
constexpr int max_price {100};	// max market price
constexpr double min_dividend_factor {.01};
constexpr double max_dividend_factor {.20};
constexpr int min_par_value {5};
constexpr int max_par_value {100};
constexpr unsigned min_fix_dividend {10};	// min fix dividend in 0.1%
constexpr unsigned max_fix_dividend {50};	// max fix dividend in 0.1%
constexpr unsigned max_minutes {60};		// max minutes in the past
constexpr unsigned max_quantity {1000};		// max quantity traded


void simple_test()
{
	std::cout << "Creating 4 stocks, 3 of them are traded \"now\" and one 200"
		<< " minutes ago.\n";

	//        stock  price	last div	fix div   	parval	common?  
	Stock S1 {"ABC", 100,	2,			0,			100,  	true};
	Stock S2 {"DEF", 150,	8,			0,			100,  	true};
	Stock S3 {"GHI", 110,	23,			0,			60,   	true};
	Stock S4 {"OLD", 12,	8,			3,			100,  	false};	
	Stock S5 {"JKL", 23,	13,			0,			250,  	true};

	//		  stock	timestamp		quantity	buy/sell	price
	Trade T1 {S1,	Clock::now(),	121,		type::buy,	S1.get_price()};
	Trade T2 {S2,	Clock::now(),	12,			type::sell, S2.get_price()};
	Trade T3 {S3,	Clock::now(),	17,			type::sell, S3.get_price()};

	auto a_long_ago {Clock::now() - std::chrono::minutes{200}};
	// the following trades won't be counted because they happened 200 min ago

	//		  stock	timestamp		quantity	buy/sell	price
	Trade T4 {S4,	a_long_ago,		30000,		type::sell, S4.get_price()};
	Trade T5 {S5,	a_long_ago,		300,		type::sell, S5.get_price()};

	std::vector<Stock> stocks {S1, S2, S3, S4, S5};
	std::vector<Trade> trades {T1, T2, T3, T4, T5};

	// Print
	std::cout << stocks << std::endl;
	std::cout << trades << std::endl;
}


// Generate N random stocks.
// Note: there is no particular reason why dividends, fixed dividends, par
// values etc. are generated according the following rules, and their values
// are not meant to be "realistic".
void random_prices()
{
	std::uniform_int_distribution stock_dist {'A', 'Z'};

	std::uniform_int_distribution price_dist {min_price, max_price};

	std::uniform_real_distribution dividend_factor_dist
									{min_dividend_factor, max_dividend_factor};

	std::uniform_int_distribution par_value_distribution
									{min_par_value, max_par_value};

	std::uniform_int_distribution fix_dividend_dist
									{min_fix_dividend, max_fix_dividend};

	std::random_device rd {"/dev/random"};	// only on Linux
	std::mt19937_64 engine {rd()};
	
	std::vector<Stock> stocks;
	stocks.reserve(N);

	for (unsigned t {0}; t < N; ++t) {
		// Create random name for stock (3 chars)
		std::string stock;	
		for (unsigned i {0}; i < 3; ++i)
			stock += stock_dist(engine);

		auto price {static_cast<unsigned>(price_dist(engine))};
		auto last_dividend
			{static_cast<unsigned>(dividend_factor_dist(engine)*price) + 1};
		auto par_value {static_cast<unsigned>(par_value_distribution(engine))};

		std::uniform_int_distribution com_pref {0, 1};
		auto common = com_pref(engine) == 0 ? true : false;

		double fixed_dividend {0.0};
		if (! common) // preferred
			fixed_dividend = fix_dividend_dist(engine)/10.0;

		stocks.emplace_back(stock,
							price,
							last_dividend,
							fixed_dividend,
							par_value,
							common);
	}

	std::cout << "RANDOM STOCKS (Div. Yield on preferred stocks can be very high\n"
		<< "because market prices are random and therefore can be much smaller\n"
		<< "than the par value.)\n";
	std::cout << stocks << std::endl;

	std::uniform_int_distribution minutes_ago {unsigned {0}, max_minutes};
	std::uniform_int_distribution quantity {unsigned {1}, max_quantity};
	std::uniform_int_distribution type {0, 1};

	std::vector<Trade> trades;
	trades.reserve(stocks.size());
	for (const auto& s : stocks) {
		auto timestamp {Clock::now()-std::chrono::minutes{minutes_ago(engine)}};
		auto q {quantity(engine)};
		auto t = type(engine) == 0 ? type::buy : type::sell;
		trades.emplace_back(s, timestamp, q, t, s.get_price());
	}

	std::cout << trades << std::endl;
}


// Inserting bad data
void bad_data_test()
{
	std::cout << "Inserting non-valid data\n";
	std::cout << "//        stock  price  last div  fix div  parval  common?\n";
	std::cout << "Stock S1 {\"ABC\", 0,     0,        0,       100,    true}\n";
	try {
		//        stock  price	last div	fix div   	parval	common?  
		Stock S1 {"ABC", 0,		0,			0,			100,  	true};
	}
	catch (std::exception& e) {
		std::cout << "Exception thrown: " << e.what() << "\n\n";
	}
	std::cout << "//        stock  price  last div  fix div  parval  common?\n";
	std::cout << "Stock S2 {\"DEF\", 10,    0,        0,       100,    false}\n";
	try {
		//        stock  price	last div	fix div   	parval	common?  
		Stock S2 {"DEF", 10,		0,			0,			100,  	false};
	}
	catch (std::exception& e) {
		std::cout << "Exception thrown: " << e.what() << "\n\n";
	}
	std::cout << "//        stock  price  last div  fix div  parval  common?\n";
	std::cout << "Stock S3 {\"ABC\", 10,    0,        0,       100,    true}\n";
	std::cout << "P/E Ratio : ";
	try {
		//        stock  price	last div	fix div   	parval	common?  
		Stock S3 {"ABC", 10,		0,			0,			100,  	true};
		std::cout << S3.pe_ratio() << std::endl;
	}
	catch (std::exception& e) {
		std::cout << "Exception thrown: " << e.what() << "\n\n";
	}
}


int main()
{
	simple_test();
	std::cout << std::endl;
	random_prices();
	std::cout << std::endl;
	bad_data_test();
}
