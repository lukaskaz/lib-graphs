#pragma once

#include "graphs/helpers.hpp"
#include "graphs/interfaces/graph.hpp"

#include <memory>

namespace graphs
{

class GraphFactory
{
  public:
    template <typename T, typename C>
    static std::shared_ptr<GraphIf> create(const C& config)
    {
        return std::shared_ptr<T>(new T(config));
    }
};

} // namespace graphs
