#include "graphs/interfaces/dygraph/rangetime/graph.hpp"

#include "webserver/helpers.hpp"
#include "webserver/interfaces/http.hpp"

#include <algorithm>
#include <fstream>
#include <future>
#include <iostream> //torem
#include <ranges>
#include <variant>

namespace graphs::dygraph::rangetime
{

class Htmlcode
{
  public:
    Htmlcode(const std::vector<std::string>& labels, const graphsize_t& size,
             std::chrono::milliseconds timems, rangetype rangetype,
             std::chrono::milliseconds rangems,
             const std::vector<std::string>& datapath) :
        refreshms{timems / 2},
        datapath{datapath}
    {
        if (this->datapath.empty())
        {
            throw std::runtime_error("Data for graph not given");
        }

        if (!labels.empty())
        {
            title = labels.at(0);
            if (labels.size() == alllabels)
            {
                xlabel = labels.at(1);
                ylabel = labels.at(2);
            }
        }

        if (size != graphsize_t{})
        {
            this->size = size;
        }

        switch (rangetype)
        {
            case rangetype::microseconds:
                range = std::chrono::duration_cast<std::chrono::microseconds>(
                            rangems)
                            .count();
                break;
            case rangetype::milliseconds:
                range = std::chrono::duration_cast<std::chrono::milliseconds>(
                            rangems)
                            .count();
                break;
            case rangetype::seconds:
                range =
                    std::chrono::duration_cast<std::chrono::seconds>(rangems)
                        .count();
                break;
        }

        htmlcode = genhtmlcode();
    }

    const std::pair<std::string, std::string>& get() const
    {
        return htmlcode;
    }

  private:
    static constexpr uint32_t alllabels{3};
    std::chrono::milliseconds refreshms{50ms};
    uint32_t range;
    const std::vector<std::string> datapath;
    std::string title{"User data graph"};
    std::string xlabel{"x axis"};
    std::string ylabel{"y axis"};
    graphsize_t size{800, 600};
    std::pair<std::string, std::string> htmlcode;

    std::pair<std::string, std::string> genhtmlcode() const
    {
        return {"\
<html>\n\
  <head>\n\
    <meta charset=\"UTF-8\">\n\
    <title>Graph " + title +
                    "</title>\n\
    <link rel=\"stylesheet\" type=\"text/css\" href=\"dygraph.css\" />\n\
    <script type=\"text/javascript\" src=\"dygraph.js\"></script>\n\
  </head>\n\
	<body>" +
                    [this](const std::vector<std::string> datapath) {
                        std::string code;
                        std::ranges::for_each(datapath,
                                              [this, &code](const auto& data) {
                                                  code += genchartcode(data);
                                              });
                        return code;
                    }(datapath) +
                    "\
        <noscript><style>body { visibility: visible; }</style></noscript>\n\
    </body></html>\n",
                "text/html"};
    }

    std::string genchartcode(const std::string& data) const
    {
        return "\
<table>\n\
	<tr>\n\
		<td valign=\"top\"><div id=\"graphall\" style=\"width:" +
               std::to_string(size.first) +
               "px; height:" + std::to_string(size.second) +
               "px; font-size:1em; padding-top:5px;\"></div></td>\n\
			<td valign=\"top\"><div id=\"legendall\"></div></td>\n\
	</tr>\n\
</table>\n\
<table>\n\
	<tr>\n\
		<td valign=\"top\"><div id=\"graphrange\" style=\"width:" +
               std::to_string(size.first) +
               "px; height:" + std::to_string(size.second) +
               "px; font-size:1em; padding-top:5px;\"></div></td>\n\
			<td valign=\"top\"><div id=\"legendrange\"></div></td>\n\
	</tr>\n\
</table>\n\
	<script type=\"text/javascript\">\n\
        Dygraph.onDOMready(function()\n\
        {\n\
            var lastMousemoveEvt = null;\n\
            var onGraphGall = false;\n\
            var onGraphGrange = false;\n\
            var samplesNum = 0;\n\
            \n\
            gall = new Dygraph(document.getElementById('graphall'),\n\
				\"" +
               data + "\", {\n\
                    title: \"" +
               title + " [samples: \" + samplesNum + \"(all)]\",\n\
				    xlabel: ' " +
               xlabel + "', \n\
				    ylabel: '" +
               ylabel + "',\n\
				    legend: 'always',\n\
				    labelsDiv: document.getElementById('legendall'),\n\
				    labelsSeparateLines: true,\n\
                    drawAxesAtZero: true,\n\
                    drawPoints: true,\n\
                    showRangeSelector: true,\n\
                    highlightCallback: function(e, x, pts, row) {\n\
                        lastMousemoveEvt = e;\n\
                        onGraphGall = true\n\
                    },\n\
                    unhighlightCallback: function(e) {\n\
                        onGraphGall = false;\n\
                    }\n\
            });\n\
            \
            grange = new Dygraph(\n\
                document.getElementById('graphrange'),\n\
				    \"" +
               data + "\", {\n\
				    title: '" +
               title + " [time: " + std::to_string(range) + "]',\n\
				    xlabel: ' " +
               xlabel + "', \n\
				    ylabel: '" +
               ylabel + "',\n\
                    dateWindow: [0, " +
               std::to_string(range) + " ],\n\
				    legend: 'always',\n\
				    labelsDiv: document.getElementById('legendrange'),\n\
				    labelsSeparateLines: true,\n\
                    drawAxesAtZero: true,\n\
                    drawPoints: true,\n\
                    showRangeSelector: true,\n\
                    highlightCallback: function(e, x, pts, row) {\n\
                        lastMousemoveEvt = e;\n\
                        onGraphGrange = true\n\
                    },\n\
                    unhighlightCallback: function(e) {\n\
                        onGraphGrange = false;\n\
                    }\n\
                }\n\
            );\n\
            \n\
            Dygraph.prototype.parseCSVall_ = (function(_super) {\n\
                return function() {\n\
                    if (arguments[0]) {\n\
                        samplesNum = arguments[0].split('\\n').length - 2;\n\
                        startAsyncCall(refreshGraphRange);\n\
                    }\n\
                    return _super.apply(this, arguments);\n\
                };\n\
            })(Dygraph.prototype.parseCSV_);\n\
            \n\
            Dygraph.prototype.parseCSVrange_ = (function(_super) {\n\
                return function() {\n\
                    if (arguments[0]) {\n\
                        startAsyncCall(refreshGraphAll);\n\
                    }\n\
                    return _super.apply(this, arguments);\n\
                };\n\
            })(Dygraph.prototype.parseCSV_);\n\
            \n\
            function getTimeMs() {\n\
                const now = new Date();\n\
                return now.getMilliseconds();\n\
            };\n\
            \n\
            refreshGraphAll = function() {\n\
                Dygraph.prototype.parseCSV_ = Dygraph.prototype.parseCSVall_;\n\
                gall.updateOptions({\n\
                    'file': \"" +
               data + "\", \n\
                    title: \"" +
               title + " [samples: \" + samplesNum + \"(all)]\",\n\
                });\n\
                if (onGraphGall) {\n\
                    gall.mouseMove_(lastMousemoveEvt);\n\
                }\n\
                console.log(getTimeMs() + \": \" + \"refreshGraphAll\");\n\
            };\n\
            \n\
            refreshGraphRange = function() {\n\
                Dygraph.prototype.parseCSV_ = Dygraph.prototype.parseCSVrange_;\n\
                var extremes = grange.xAxisExtremes();\n\
                var rangestart = grange.xAxisExtremes()[0];\n\
                var rangeend = grange.xAxisExtremes()[1];\n\
                if ((rangeend - rangestart) > " +
               std::to_string(range) + ") {\n\
                    rangestart = rangeend - " +
               std::to_string(range) + ";\n\
                }\n\
                grange.updateOptions({\n\
                    'file': \"" +
               data + "\", \n\
                    'dateWindow': [rangestart, rangeend]\n\
                });\n\
                if (onGraphGrange) {\n\
                    grange.mouseMove_(lastMousemoveEvt);\n\
                }\n\
                console.log(getTimeMs() + \": \" + \"refreshGraphRange\");\n\
            };\n\
            \n\
            (startAsyncCall = function(call, time = " +
               std::to_string(refreshms.count()) + ") {\n\
                setTimeout(call, time);\n\
            })(refreshGraphRange);\n\
        });\n\
</script>\n";
    }
};

struct Graph::Handler
{
  public:
    Handler(const configshort_t& config) :
        labels{std::get<std::vector<std::string>>(config)},
        size{std::get<graphsize_t>(config)},
        params{std::get<graphparamsshort_t>(config)}, server{getserver()},
        data{std::get<3>(std::get<graphparamsshort_t>(params))},
        html{labels,
             size,
             std::get<0>(std::get<graphparamsshort_t>(params)),
             std::get<1>(std::get<graphparamsshort_t>(params)),
             std::get<2>(std::get<graphparamsshort_t>(params)),
             data.getnames()}
    {
        init(labels);
    }

    Handler(const configall_t& config) :
        labels{std::get<std::vector<std::string>>(config)},
        size{std::get<graphsize_t>(config)},
        params{std::get<graphparamsall_t>(config)}, server{getserver()},
        data{std::get<3>(std::get<graphparamsall_t>(params))},
        html{labels,
             size,
             std::get<0>(std::get<graphparamsall_t>(params)),
             std::get<1>(std::get<graphparamsall_t>(params)),
             std::get<2>(std::get<graphparamsall_t>(params)),
             data.getnames()}
    {
        init(labels);
    }

    ~Handler()
    {
        stop();
    }

    void start()
    {
        shareresources();
        sendhtmlcode();
        std::cout << "[INFO] " << startmsg << std::endl;
        async = std::async(std::launch::async, [this]() { server.listen(); });
    }

    void stop()
    {
        server.close();
    }

    void add(const std::string& entry)
    {
        data.add(entry);
    }

  private:
    using files_t =
        std::vector<std::tuple<std::string, std::string, std::string>>;
    static constexpr std::string path{"graph"};
    uint16_t port{9000};
    const std::vector<std::string> labels;
    const graphsize_t size;
    const std::variant<graphparamsshort_t, graphparamsall_t> params;
    http::Server server;
    helpers::CircularCollection data;
    Htmlcode html;
    const files_t files = {
        {"../resources/", "dygraph.js", "text/javascript"},
        {"../resources/", "dygraph.css", "text/css"},
        {"../resources/", "favicon.ico", "image/vnd.microsoft.icon"}};
    std::string startmsg;
    std::future<void> async;

    void init(const std::vector<std::string>& labels)
    {
        std::string name{"Unnamed"};
        if (!labels.empty())
            name = labels.at(0);
        const auto [iface, ipaddr] = getipaddr();
        startmsg = "Graph [" + name + "] available under address(" + iface +
                   "): " + ipaddr + ":" + std::to_string(port) + "/" + path;
    }

    http::Server getserver()
    {
        while (port < std::numeric_limits<std::uint16_t>::max())
        {
            try
            {
                return http::Server(port);
            }
            catch (...)
            {
                port++;
            }
        }
        throw std::runtime_error("No available port for new graph");
    }

    std::pair<std::string, std::string> getipaddr()
    {
        static const std::vector<std::string> ifaces{"eth0", "eth1", "enp0s0",
                                                     "enp1s0", "enp2s0"};
        std::string ipaddress;
        for (const auto& iface : ifaces)
        {
            if (auto ipaddr = http::getIPAddress(iface); !ipaddr.empty())
            {
                return {iface, ipaddr};
            }
        }
        throw std::runtime_error("Cannot find host ip address");
    }

    void sendhtmlcode()
    {
        server.get("/" + path, [&html = html.get()](auto, auto res) {
            const auto& [code, type] = html;
            res.headers.push_back("Content-Type: " + type);
            res >> code;
        });
    }

    void shareresources()
    {
        std::ranges::for_each(files, [this](const auto& file) {
            const auto& [path, name, type] = file;
            const auto filepath = path + name;
            std::ifstream ifs(filepath);
            if (!ifs.is_open())
            {
                throw std::runtime_error("Cannot open " + filepath);
            }
            auto content =
                std::string(std::istreambuf_iterator<char>(ifs.rdbuf()), {});
            server.get("/" + name,
                       [content = content, type, name](auto, auto res) {
                           res.headers.push_back("Content-Type: " + type);
                           res >> content;
                       });
        });

        std::ranges::for_each(data.getelems(), [this](const auto& elem) {
            const auto& [name, type] = elem.getinfo();
            server.get("/" + name, [&elem, type](auto, auto res) {
                res.headers.push_back("Content-Type: text/csv");
                res >> elem.getdata();
            });
        });
    }
};

Graph::Graph(const config_t& config)
{
    handler = std::visit(
        [](const auto& config) -> decltype(Graph::handler) {
            if constexpr (!std::is_same<const std::monostate&,
                                        decltype(config)>())
            {
                return std::make_unique<Graph::Handler>(config);
            }
            throw std::runtime_error("Graph config not supported");
        },
        config);
}
Graph::~Graph() = default;

void Graph::start()
{
    handler->start();
}

void Graph::stop()
{
    handler->stop();
}

void Graph::add(const std::string& entry)
{
    handler->add(entry);
}

} // namespace graphs::dygraph::rangetime
