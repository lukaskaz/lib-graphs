#include "graphs/interfaces/dygraph.hpp"

#include "webserver/helpers.hpp"
#include "webserver/interfaces/http.hpp"

#include <algorithm>
#include <fstream>
#include <future>
#include <iostream> //torem
#include <ranges>

namespace graphs::dygraph
{

class Htmlcode
{
  public:
    Htmlcode(const std::vector<std::string>& labels, const graphsize_t& size,
             std::chrono::milliseconds timems, uint32_t range,
             const std::vector<std::string>& datapath) :
        refreshms{timems}, rangesize{range}, datapath{datapath}
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

        htmlcode = genhtmlcode();
    }

    const std::pair<std::string, std::string>& get() const
    {
        return htmlcode;
    }

  private:
    static constexpr uint32_t alllabels{3};
    std::chrono::milliseconds refreshms{50};
    uint32_t rangesize{50};
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
    <title>Data presentation</title>\n\
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
               title + " [samples: " + std::to_string(rangesize) + "]',\n\
				    xlabel: ' " +
               xlabel + "', \n\
				    ylabel: '" +
               ylabel + "',\n\
				    legend: 'always',\n\
				    labelsDiv: document.getElementById('legendrange'),\n\
				    labelsSeparateLines: true,\n\
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
            Dygraph.prototype.parseCSVnative_ = (function(_super) {\n\
                return function() {\n\
                    if (arguments[0]) {\n\
                        samplesNum = arguments[0].split('\\n').length;\n\
                        setDeferredCall(refreshGraphRange);\n\
                    }\n\
                    return _super.apply(this, arguments);\n\
                };\n\
            })(Dygraph.prototype.parseCSV_);\n\
            \n\
            Dygraph.prototype.parseCSVrange_ = (function(_super) {\n\
                return function() {\n\
                    if (arguments[0]) {\n\
                        var values = arguments[0].split('\\n');\n\
                        var entriestokeep = " +
               std::to_string(rangesize) + ";\
                        var keepidx = values.length - entriestokeep;\n\
                        if(keepidx < 0) keepidx = 0;\n\
                        values = values.filter(function(_, i) {\n\
                            return i == 0 || i >= keepidx;\n\
                        });\n\
                        arguments[0] = values.join([separator='\\n']);\n\
                        setDeferredCall(refreshGraphAll);\n\
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
                Dygraph.prototype.parseCSV_ = Dygraph.prototype.parseCSVnative_;\n\
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
                grange.updateOptions({\n\
                'file': \"" +
               data + "\"});\n\
                if (onGraphGrange) {\n\
                    grange.mouseMove_(lastMousemoveEvt);\n\
                }\n\
                console.log(getTimeMs() + \": \" + \"refreshGraphRange\");\n\
            };\n\
            \n\
            (setDeferredCall = function(call, time = " +
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
    Handler(const std::vector<std::string>& labels, const graphsize_t& size,
            const graphparamsall_t& params) :
        server{[]() {
            port++;
            return port;
        }()},
        data{std::get<dataparamsall_t>(params)},
        html{labels, size, std::get<std::chrono::milliseconds>(params),
             std::get<uint32_t>(params), data.getnames()}
    {
        init(labels);
    }
    Handler(const std::vector<std::string>& labels, const graphsize_t& size,
            const graphparamsshort_t& params) :
        server{[]() {
            port++;
            return port;
        }()},
        data{std::get<dataparamsshort_t>(params)},
        html{labels, size, std::get<std::chrono::milliseconds>(params),
             std::get<uint32_t>(params), data.getnames()}
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

  private:
    using files_t =
        std::vector<std::tuple<std::string, std::string, std::string>>;
    static constexpr std::string path{"graph"};
    static uint16_t port;
    http::Server server;
    CircularCollection data;
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
        auto ip = http::getIPAddress("eth0");
        startmsg = "Graph [" + name + "] available under address: " + ip + ":" +
                   std::to_string(port) + "/" + path;
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
            server.get("/" + name, [file = path + name, &type](auto, auto res) {
                std::ifstream ifs(file);
                if (!ifs.is_open())
                {
                    throw std::runtime_error("Cannot open " + file);
                }
                auto content = std::string(
                    std::istreambuf_iterator<char>(ifs.rdbuf()), {});
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
uint16_t Graph::Handler::port{9000};

Graph::Graph(const std::vector<std::string>& labels, const graphsize_t& size,
             const graphparamsall_t& params) :
    handler{std::make_unique<Handler>(labels, size, params)}
{}
Graph::Graph(const std::vector<std::string>& labels, const graphsize_t& size,
             const graphparamsshort_t& params) :
    handler{std::make_unique<Handler>(labels, size, params)}
{}
Graph::~Graph() = default;

void Graph::start()
{
    handler->start();
}

void Graph::stop()
{
    handler->stop();
}

} // namespace graphs::dygraph
