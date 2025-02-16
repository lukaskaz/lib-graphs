#pragma once

#include "graphs/factory.hpp"

#include <tuple>
#include <variant>

namespace graphs::dygraph::rangetime
{

enum class rangetype
{
    microseconds,
    milliseconds,
    seconds
};

using graphparamsall_t = std::tuple<std::chrono::milliseconds, rangetype,
                                    std::chrono::milliseconds, dataparamsall_t>;

using graphparamsshort_t =
    std::tuple<std::chrono::milliseconds, rangetype, std::chrono::milliseconds,
               dataparamsshort_t>;

using configshort_t =
    std::tuple<std::vector<std::string>, graphsize_t, graphparamsshort_t>;
using configall_t =
    std::tuple<std::vector<std::string>, graphsize_t, graphparamsall_t>;

using config_t = std::variant<configall_t, configshort_t>;

class Graph : public GraphIf
{
  public:
    ~Graph();
    void start() override;
    void stop() override;
    void add(const std::string&) override;

  private:
    friend class graphs::Factory;
    Graph(const config_t&);

    struct Handler;
    std::unique_ptr<Handler> handler;
};

} // namespace graphs::dygraph::rangetime
