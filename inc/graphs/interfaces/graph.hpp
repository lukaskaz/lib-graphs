#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

using namespace std::chrono_literals;

namespace graphs
{

using dataparamsall_t =
    std::vector<std::tuple<std::string, std::string, uint32_t>>;
using dataparamsshort_t = std::vector<std::tuple<std::string, std::string>>;
using graphsize_t = std::pair<uint32_t, uint32_t>;

class GraphIf
{
  public:
    virtual ~GraphIf() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void add(const std::string&) = 0;
};

} // namespace graphs
