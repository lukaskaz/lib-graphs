#include "graphs/interfaces/dygraph.hpp"

#include <iostream>
#include <memory>

int main()
{
    try
    {
        using namespace graphs::dygraph;
        auto graphA = graphs::GraphFactory::create<Graph>(
            {"Device A status", "label x", "label y"}, {500, 250},
            {100ms, 100, {{"dataa.csv", "time,state"}}});
        auto graphB = graphs::GraphFactory::create<Graph>(
            {"Device B status", "label x", "label y"}, {500, 250},
            {100ms, 100, {{"datab.csv", "time,state", 1000}}});
        // graphA->start();
        // graphB->start();
    }
    catch (std::exception& err)
    {
        std::cerr << "[ERROR] " << err.what() << '\n';
    }
    return 0;
}
