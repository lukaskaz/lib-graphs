#include "graphs/interfaces/dygraph/rangetime/graph.hpp"

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
            using namespace graphs::dygraph::rangetime;
            auto graphA = graphs::GraphFactory::create<Graph, configall_t>(
                {{"Sine/cosine", "position [degree/time]",
                  "sin/cos value [-1,1]"},
                 {1200, 400},
                 {100ms,
                  rangetype::seconds,
                  180s,
                  {{"dataa.csv", "pos/timems,sin,cos", 1000}}}});
            auto graphB = graphs::GraphFactory::create<Graph, configshort_t>(
                {{
                     "Asine/acosine",
                     "asin/acos value/time [-1,...]",
                     "position [degree]",
                 },
                 {1200, 400},
                 {100ms,
                  rangetype::seconds,
                  2s,
                  {{"datab.csv", "pos/timems,asin,acos"}}}});
            graphA->start();
            graphB->start();

            std::ranges::for_each(
                std::views::iota(0, 200),
                [&graphA, &graphB,
                 tm = graphs::helpers::TimeMonitor()](const auto) {
                    double rawtimems = tm.getmiliseconds() - 1000.f,
                           rawtimesec = rawtimems / 1000.f,
                           apos = rawtimesec * 180.f,
                           bpos =
                               ((tm.getmiliseconds() % 2000) - 1000.f) / 1000.f;

                    if (rawtimesec > 0 &&
                        std::fabs(bpos - (-1.0f)) <=
                            std::numeric_limits<decltype(bpos)>::epsilon())
                    {
                        auto rawtimesecsupp = rawtimesec - 0.01f,
                             bpossupp = bpos - (-2.f);
                        auto asin = (std::asin(bpossupp) * 180.f) /
                                    std::numbers::pi_v<double>,
                             acos = (std::acos(bpossupp) * 180.f) /
                                    std::numbers::pi_v<double>;
                        auto recordBsupp = std::to_string(rawtimesecsupp) +
                                           "," + std::to_string(asin) + "," +
                                           std::to_string(acos);
                        graphB->add(recordBsupp);
                    }

                    auto sin =
                             std::sin(std::numbers::pi_v<double> * rawtimesec),
                         cos =
                             std::cos(std::numbers::pi_v<double> * rawtimesec),
                         asin = (std::asin(bpos) * 180.f) /
                                std::numbers::pi_v<double>,
                         acos = (std::acos(bpos) * 180.f) /
                                std::numbers::pi_v<double>;

                    auto recordA = std::to_string(apos) + "," +
                                   std::to_string(sin) + "," +
                                   std::to_string(cos),
                         recordB = std::to_string(rawtimesec) + "," +
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
