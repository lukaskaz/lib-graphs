#include "graphs/interfaces/dygraph/rangesamples/graph.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <numbers>
#include <ranges>

int main(int argc, char**)
{
    if (argc > 1)
    {
        try
        {
            using namespace graphs::dygraph::rangesamples;
            auto graphA = graphs::GraphFactory::create<Graph, configall_t>(
                {{"Sine/cosine", "position [degree]", "sin/cos value [-1,1]"},
                 {500, 250},
                 {100ms, 100, {{"dataa.csv", "pos,sin,cos", 1000}}}});
            auto graphB = graphs::GraphFactory::create<Graph, configshort_t>(
                {{"Arcsine/arccosine", "position [degree]",
                  "sin/cos value [-1,...]"},
                 {500, 250},
                 {100ms, 100, {{"datab.csv", "pos,asin,acos"}}}});
            graphA->start();
            graphB->start();

            // graphs::helpers::TimeMonitor tm;
            std::ranges::for_each(
                std::views::iota(0, 250),
                [&graphA, &graphB,
                 tm = graphs::helpers::TimeMonitor()](const auto) {
                    double rawtime = (tm.getmiliseconds() - 1000.f) / 1000.f,
                           apos = rawtime * 180.f,
                           bpos =
                               ((tm.getmiliseconds() % 2000) - 1000.f) / 1000.f,
                           sin = std::sin(std::numbers::pi_v<double> * rawtime),
                           asin = (std::asin(bpos) * 180.f) /
                                  std::numbers::pi_v<double>,
                           cos = std::cos(std::numbers::pi_v<double> * rawtime),
                           acos = (std::acos(bpos) * 180.f) /
                                  std::numbers::pi_v<double>;
                    std::string recordA = std::to_string(apos) + "," +
                                          std::to_string(sin) + "," +
                                          std::to_string(cos);
                    std::string recordB = std::to_string(rawtime) + "," +
                                          std::to_string(asin) + "," +
                                          std::to_string(acos);
                    graphA->add(recordA);
                    graphB->add(recordB);

                    static const uint32_t delayms{25};
                    auto remaindelay = delayms - tm.getmiliseconds() % delayms;
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
