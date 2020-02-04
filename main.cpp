#include <iomanip>
#include "luenberger_ch13ex14.h"


std::map<std::string, double> scan_arguments(int narg, char* args[])
{
    std::string argsLine;
    for (int i {1}; i < narg; ++i)
        argsLine += args[i];

    // default values
    constexpr double T     {1};
    constexpr double S     {1};
    constexpr int    p     {12};
    constexpr double mu    {0.1};
    constexpr double sigma {0.3};

    // pattern for the flags
    std::string tString     {R"(-T\s*(\d+))"};    // accepts only ints
    std::string sString     {R"(-S\s*(\d+(\.\d+)?))"};
    std::string pString     {R"(-p\s*(\d+))"};    // accepts only ints
    std::string muString    {R"(-m\s*(\d+(\.\d+)?))"};
    std::string sigmaString {R"(-s\s*(\d+(\.\d+)?))"};

    std::vector<std::pair <std::string, double>> matches {
        {tString, T}, {sString, S}, {pString, p},
        {muString, mu}, {sigmaString, sigma}
    };

    std::transform(matches.begin(), matches.end(), matches.begin(),
                   [&argsLine](auto& p)
                   {
                        if (std::smatch m; regex_search(argsLine, m, std::regex {p.first}))
                            p.second = stod(m[1]);

                        return p;
                   });

    std::map<std::string, double> params {
        {"T",       matches[0].second},
        {"S",       matches[1].second},
        {"p",       matches[2].second},
        {"mu",      matches[3].second},
        {"sigma",   matches[4].second}
    };

    return params;
}


int main(int argc, char* argv[])
{
    auto parameters {scan_arguments(argc, argv)};

    double          mu       {parameters["mu"]};
    double          sigma    {parameters["sigma"]};
    GeomBrownMotion gbm      {mu, sigma};

    double years   {parameters["T"]};
    double s0      {parameters["S"]};
    double periods {parameters["p"]};
    PricesParam pp {s0, static_cast<unsigned long>(years),
                    static_cast<unsigned>(periods), gbm};

    unsigned long seed {std::random_device{"/dev/urandom"}()};

    unsigned      p  {pp.get_periods()};
    double        dt {1.0/p};
    unsigned long y  {pp.get_years()};
    double        eps;

    std::normal_distribution<> normalDist;
    std::default_random_engine dre {seed};

    double latestLinearPrice {s0};
    double latestLogPrice    {s0};

    std::ofstream ofs {"Prices"};

    ofs << "t" << '\t' << "linear_prices" << '\t' << "logarithmic_prices" << '\n';
    ofs << std::left << std::setfill(' ') << std::setw(10) << 0
        << '\t' << std::left << std::setw(10) << latestLinearPrice
        << '\t' << std::setw(10) << latestLogPrice << '\n';

    for (unsigned long t {1}; t <= y*p; ++t) {
        eps = normalDist(dre);
        latestLinearPrice = linear_advance(latestLinearPrice, mu, dt, sigma, eps);
        latestLogPrice = logarithmic_advance(latestLogPrice, mu, dt, sigma, eps);
        ofs    << std::left << std::setfill(' ') << std::setw(10) << t
            << '\t' << std::left << std::setw(10) << latestLinearPrice
            << '\t' << std::setw(10) << latestLogPrice << '\n';
    }

    ofs.close();

    constexpr unsigned int numberOfTries {1000};
    double totFinalLin {0};
    double totFinalLog {0};

    for (unsigned n {0}; n < numberOfTries; ++n) {
        Prices lp   {pp, Integration::Linear};
        Prices logp {pp, Integration::Logarithmic};
        lp.generate_prices(seed);
        logp.generate_prices(seed);
        totFinalLin += lp.final_price();
        totFinalLog += logp.final_price();
    }

    std::cout << "Final Price Average: "
        << totFinalLin/numberOfTries << '\n';
    std::cout << "Final LogPrice Aver: "
        << totFinalLog/numberOfTries << '\n';

    return 0;
}
