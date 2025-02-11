#include "graphs/interfaces/dygraph/rangesamples/graph.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <numbers>
#include <random>
#include <ranges>

int main(int argc, char**)
{
    if (argc > 1)
    {
        try
        {
            using namespace graphs::dygraph::rangesamples;
            auto graphA = graphs::GraphFactory::create<Graph, configall_t>(
                {{"sine/cosine", "position [degree]", "sin/cos value [-1,1]"},
                 {1200, 400},
                 {100ms, 25, {{"dataa.csv", "pos,sin,cos", 1000}}}});
            auto graphB = graphs::GraphFactory::create<Graph, configshort_t>(
                {{
                     "asine/acosine",
                     "asin/acos value [-1,...]",
                     "position [degree]",
                 },
                 {1200, 400},
                 {100ms, 25, {{"datab.csv", "pos,asin,acos"}}}});
            graphA->start();
            graphB->start();

            // use randomly generated delays(ms) in given range
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<uint32_t> distrib(50, 250);

            std::ranges::for_each(
                std::views::iota(0, 150),
                [&graphA, &graphB, &distrib, &gen,
                 tm = graphs::helpers::TimeMonitor()](const auto) {
                    double rawtime = (tm.getmiliseconds() - 1000.f) / 1000.f,
                           apos = rawtime * 180.f,
                           bpos =
                               ((tm.getmiliseconds() % 2000) - 1000.f) / 1000.f;

                    // include ending corner case for asine/acosine
                    if (rawtime > 0 &&
                        std::fabs(bpos - (-1.0f)) <=
                            std::numeric_limits<decltype(bpos)>::epsilon())
                    {
                        auto rawtimsupp = rawtime - 0.01f,
                             bpossupp = bpos - (-2.f);
                        auto asin = (std::asin(bpossupp) * 180.f) /
                                    std::numbers::pi_v<double>,
                             acos = (std::acos(bpossupp) * 180.f) /
                                    std::numbers::pi_v<double>;
                        auto recordBsupp = std::to_string(rawtimsupp) + "," +
                                           std::to_string(asin) + "," +
                                           std::to_string(acos);
                        graphB->add(recordBsupp);
                    }

                    auto sin = std::sin(std::numbers::pi_v<double> * rawtime),
                         cos = std::cos(std::numbers::pi_v<double> * rawtime),
                         asin = (std::asin(bpos) * 180.f) /
                                std::numbers::pi_v<double>,
                         acos = (std::acos(bpos) * 180.f) /
                                std::numbers::pi_v<double>;

                    auto recordA = std::to_string(apos) + "," +
                                   std::to_string(sin) + "," +
                                   std::to_string(cos),
                         recordB = std::to_string(rawtime) + "," +
                                   std::to_string(asin) + "," +
                                   std::to_string(acos);
                    graphA->add(recordA);
                    graphB->add(recordB);

                    auto delayms{distrib(gen)},
                        remaindelay = delayms - tm.getmiliseconds() % delayms;
                    usleep(remaindelay * 1000);
                });

            while (true)
                usleep(100 * 1000);
        }
        catch (std::exception& err)
        {
            std::cerr << "[ERROR] " << err.what() << '\n';
        }
    }

    return 0;
}
