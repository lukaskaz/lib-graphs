#pragma once

#include "graphs/helpers.hpp"
#include "graphs/interfaces/graph.hpp"

#include <memory>

namespace graphs
{

class GraphFactory
{
  public:
    template <typename T>
    static std::shared_ptr<GraphIf>
        create(const std::vector<std::string>& labels, const graphsize_t& size,
               const graphparamsall_t& params)
    {
        return std::shared_ptr<T>(new T(labels, size, params));
    }

    template <typename T>
    static std::shared_ptr<GraphIf>
        create(const std::vector<std::string>& labels, const graphsize_t& size,
               const graphparamsshort_t& params)
    {
        return std::shared_ptr<T>(new T(labels, size, params));
    }
};

} // namespace graphs
