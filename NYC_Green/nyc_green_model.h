#pragma once

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <nlohmann/json.hpp>
#include <locale>
#include <codecvt>
#include <memory>


using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

#define NREL_API_TOKEN "9MrGLHY23Kvs8LV8XHYpPsJRQZrezg3GJl18burh"
#define GEO_API_TOKEN "pk.eyJ1IjoiYWdhcmM0NDkiLCJhIjoiY2t3bzkxZnRiMDBsbjJ1czZyeWMyZ2J2dSJ9.Bq6qVP8nIQu8Sv_aZbN3sg"

/**
* Base class for making JSON API requests.
*/
class NetworkRequest {
public:
	/**
	* Appends an HTTP query to the request
	* 
	* @param key HTTP query key
	* @param value HTTP key value
	*/
	void append_query(std::wstring key, std::wstring value);
	/**
	* Appends an HTTP path to the base URI
	*
	* @param value HTTP path string
	*/
	void append_path(std::wstring value);
	/**
	* Spawns a thread to create a request using the completed URI & HTTP queries
	*
	*/
	void request();
	/**
	* Returns the body of the response in a JSON format
	* 
	* @return nlohmann::json response
	*/
	nlohmann::json get_json();
	/**
	* 
	* @return true if request was successful
	*/
	bool is_successful();
protected:
	std::unique_ptr<http_client> client;
	std::unique_ptr<uri_builder> builder;
	nlohmann::json json_response;
	bool success = false;
};

/**
* Interface to NREL's weather data API
*/
class NRELRequest : public NetworkRequest {
public:
	NRELRequest() = delete;
	NRELRequest(std::wstring base_uri, std::wstring query);
};

/**
* Interface to mapbox's geocoding API.
*/
class GeocodeRequest : public NetworkRequest {
public:
	GeocodeRequest() = delete;
	GeocodeRequest(std::wstring base_uri, std::wstring query);
};