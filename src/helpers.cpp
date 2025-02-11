#include "graphs/helpers.hpp"

#include <arpa/inet.h>
#include <ifaddrs.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <ranges>
#include <sstream>

namespace graphs::helpers
{

TimeMonitor::TimeMonitor() : start{std::chrono::steady_clock::now()}
{}

uint32_t TimeMonitor::getmiliseconds() const
{
    auto diff = std::chrono::steady_clock::now() - start;
    return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
}

uint32_t TimeMonitor::getseconds() const
{
    auto diff = std::chrono::steady_clock::now() - start;
    return std::chrono::duration_cast<std::chrono::seconds>(diff).count();
}

std::string TimeMonitor::gettimestamp() const
{
    const auto currtimepoint{std::chrono::system_clock::now()};
    const auto currtime{std::chrono::system_clock::to_time_t(currtimepoint)};
    const auto currlocaltime{*std::localtime(&currtime)};
    const auto currtimeepoch{currtimepoint.time_since_epoch()};
    const auto currtimems{
        duration_cast<std::chrono::milliseconds>(currtimeepoch).count() % 1000};

    std::ostringstream oss;
    oss << std::put_time(&currlocaltime, "%F %T") << "." << std::setw(3)
        << std::setfill('0') << currtimems;

    return oss.str();
}

CircularData::CircularData(const std::string& name, const std::string& header) :
    CircularData{name, header, std::numeric_limits<decltype(limit)>::max()}
{}

CircularData::CircularData(const std::string& name, const std::string& header,
                           uint32_t limit) :
    name{name},
    header{header}, limit{limit}
{}
const std::string CircularData::type{"text/csv"};

void CircularData::add(const std::string& entry)
{
    queue.push_back(entry + "\n");
    while (queue.size() > limit)
        queue.pop_front();
}

std::pair<std::string, std::string> CircularData::getinfo() const
{
    return {name, type};
}

std::string CircularData::getname() const
{
    return name;
}

std::string CircularData::getdata() const
{
    std::string output{header + "\n"};
    std::ranges::for_each(
        queue, [this, &output](const auto& entry) { output += entry; });
    return output;
}

CircularCollection::CircularCollection(const dataparamsall_t& params)
{
    std::ranges::for_each(params, [this](const auto& param) {
        const auto& [name, header, limit] = param;
        elems.emplace_back(name, header, limit);
    });
}
CircularCollection::CircularCollection(const dataparamsshort_t& params)
{
    std::ranges::for_each(params, [this](const auto& param) {
        const auto& [name, header] = param;
        elems.emplace_back(name, header);
    });
}

void CircularCollection::add(const std::string& entry)
{
    std::ranges::for_each(elems, [&entry](auto& item) { item.add(entry); });
}

std::vector<std::string> CircularCollection::getnames() const
{
    std::vector<std::string> names;
    std::ranges::for_each(
        elems, [&names](const auto& item) { names.push_back(item.getname()); });
    return names;
}

const std::vector<CircularData>& CircularCollection::getelems() const
{
    return elems;
}

} // namespace graphs::helpers
