#include "graphs/interfaces/dygraph/rangesamples/graph.hpp"

#include <iostream>
#include <memory>

int main()
{
    try
    {
        using namespace graphs::dygraph::rangesamples;
        auto graphA = graphs::GraphFactory::create<Graph, configshort_t>(
            {{"Device A status", "time [ms]", "value [unit]"},
             {500, 250},
             {100ms, 100, {{"dataa.csv", "time,value"}}}});
        auto graphB = graphs::GraphFactory::create<Graph, configall_t>(
            {{"Device B status", "label x", "label y"},
             {500, 250},
             {100ms, 100, {{"datab.csv", "time,state", 1000}}}});
        // graphA->start();
        // graphB->start();

        // graphs::helpers::TimeMonitor tm;
        // std::string data = std::to_string(tm.getseconds()) + "," + "200";
        // graphA->add(data);
        // sleep(1);
        // data = std::to_string(tm.getseconds()) + "," + "100";
        // graphA->add(data);
        // sleep(1);
        // data = std::to_string(tm.getseconds()) + "," + "300";
        // graphA->add(data);
        // while (true)
        //     usleep(100 * 1000);
    }
    catch (std::exception& err)
    {
        std::cerr << "[ERROR] " << err.what() << '\n';
    }
    return 0;
}
