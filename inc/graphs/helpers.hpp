#pragma once

#include "graphs/interfaces/graph.hpp"

#include <deque>
#include <string>
#include <vector>

namespace graphs::helpers
{

class TimeMonitor
{
  public:
    TimeMonitor();
    uint32_t getseconds() const;
    uint32_t getmiliseconds() const;
    std::string gettimestamp() const;

  private:
    std::chrono::steady_clock::time_point start;
};

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

} // namespace graphs::helpers
