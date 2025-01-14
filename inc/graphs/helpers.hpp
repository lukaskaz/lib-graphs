#pragma once

#include "graphs/interfaces/graph.hpp"

#include <deque>
#include <string>
#include <vector>

namespace graphs
{

std::string gettimestamp();

class CircularData
{
  public:
    CircularData(const std::string&, const std::string&);
    CircularData(const std::string&, const std::string&, uint32_t);

    void add(const std::string& entry);
    std::pair<std::string, std::string> getinfo() const;
    std::string getname() const;
    std::string getdata() const;

  private:
    static const std::string type;
    const std::string name;
    const std::string header;
    const uint32_t limit;
    std::deque<std::string> queue;
};

class CircularCollection
{
  public:
    CircularCollection(const dataparamsall_t& params);
    CircularCollection(const dataparamsshort_t& params);

    void add(const std::string& entry);
    std::vector<std::string> getnames() const;
    const std::vector<CircularData>& getelems() const;

  private:
    std::vector<CircularData> elems;
};

} // namespace graphs
