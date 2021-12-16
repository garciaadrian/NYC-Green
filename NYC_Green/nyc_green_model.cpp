#include "nyc_green_model.h"


NRELRequest::NRELRequest(std::wstring base_uri, std::wstring query) {
	client = std::make_unique<http_client>(base_uri);
    builder = std::make_unique<uri_builder>(query);
    append_query(U("api_key"), U(NREL_API_TOKEN)); //

}

void NetworkRequest::append_query(std::wstring key, std::wstring value) {
    builder->append_query(key, value);
}

void NetworkRequest::append_path(std::wstring value) {
    builder->append_path(value);
}

void NetworkRequest::request() {
    std::wstring request = builder->to_string();

    pplx::task<http_response> task = client->request(methods::GET, builder->to_string())
        .then([](http_response response) {
        if (response.status_code() == status_codes::OK) {
            response.content_ready().wait();
            return response;
        }
        });

    try {
        while (!task.is_done()) { std::cout << "*"; }
    }
    catch (const std::exception& e) {
        std::cout << e.what();
    }

    http_response response = task.get();

    pplx::task<std::wstring> get_string([response]() {
        return response.extract_string(true);
        });

    try {
        while (!get_string.is_done()) { std::cout << "."; }
    }
    catch (const std::exception& e) {
        std::cout << e.what();
    }

    std::wstring r = get_string.get();

    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    std::string converted_str = converter.to_bytes(r);

    this->json_response = nlohmann::json::parse(converted_str);
    this->success = true;
}

nlohmann::json NetworkRequest::get_json() {
    return json_response;
}

bool NetworkRequest::is_successful() {
    return success;
}

GeocodeRequest::GeocodeRequest(std::wstring base_uri, std::wstring query) {
    client = std::make_unique<http_client>(base_uri);
    builder = std::make_unique<uri_builder>(query);
    append_query(U("access_token"), U(GEO_API_TOKEN));
    append_query(U("country"), U("US"));
}