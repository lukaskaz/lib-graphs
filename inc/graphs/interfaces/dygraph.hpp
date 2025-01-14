#pragma once

#include "graphs/factory.hpp"

namespace graphs::dygraph
{

class Graph : public GraphIf
{
  public:
    ~Graph();
    void start() override;
    void stop() override;
    void add(const std::string&) override;

  private:
    friend class graphs::GraphFactory;
    Graph(const std::vector<std::string>&, const graphsize_t&,
          const graphparamsall_t&);
    Graph(const std::vector<std::string>&, const graphsize_t&,
          const graphparamsshort_t&);

    struct Handler;
    std::unique_ptr<Handler> handler;
};

} // namespace graphs::dygraph
