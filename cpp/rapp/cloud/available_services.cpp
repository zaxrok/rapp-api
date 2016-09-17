#include "available_services.hpp"

namespace rapp {
namespace cloud {

available_services::available_services(std::function<void(std::vector<service>)> callback)
: http_request("GET /hop/available_services HTTP/1.1\r\n"),
  delegate_(callback)
{
    assert(callback);
}

void available_services::deserialise(std::string json) 
{
    if (json.empty()) {
        throw std::runtime_error("empty json reply");
    }
    nlohmann::json json_f;
    try {
        json_f = json::parse(json);
    }
    catch (std::exception & e) {
        std::cerr << "Exception " << e.what() << std::endl;
    }
    auto error = misc::get_json_value<std::string>("error", json_f);
    if (!error.empty()) {
         std::cerr << "error JSON: " << error << std::endl; 
    }
    else {
        std::vector<service> services;
        for (auto it_s : json_f["services"]) {
            services.push_back(std::make_pair(misc::get_json_value<std::string>("name", it_s), 
                                              misc::get_json_value<std::string>("url", it_s)));
        }
        delegate_(services);
    }
}

}
}