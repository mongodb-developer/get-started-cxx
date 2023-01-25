#pragma once

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/list.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include <iostream>
#include <cpr/cpr.h>

using namespace std;

std::string getEnvironmentVariable(std::string environmentVarKey)
{
	char* pBuffer = nullptr;
	size_t size = 0;
	auto key = environmentVarKey.c_str();
	
	// Use the secure version of getenv, ie. _dupenv_s to fetch environment variable.  
	if (_dupenv_s(&pBuffer, &size, key) == 0 && pBuffer != nullptr)
	{
		std::string environmentVarValue(pBuffer);
		free(pBuffer);
		return environmentVarValue;
	}
	else
	{
		return "";
	}
}

int getAQI(std::string city, std::string apiToken)
{
	// Call the API to get the air quality index.
	std::string aqiUrl = "https://api.waqi.info/feed/" + city + "/?token=" + apiToken;
	auto aqicnResponse = cpr::Get(cpr::Url{ aqiUrl });

	// Get the AQI from the response
	if(aqicnResponse.text.empty())
	{
		cout << "Error: Response is empty." << endl;
		return -1;
	}
	bsoncxx::document::value aqicnResponseBson = bsoncxx::from_json(aqicnResponse.text);
	auto aqi = aqicnResponseBson.view()["data"]["aqi"].get_int32().value;
	return aqi;
}

void saveToCollection(mongocxx::collection& collection, int aqi)
{
	auto timeStamp = bsoncxx::types::b_date(std::chrono::system_clock::now());

	bsoncxx::builder::stream::document aqiDoc = bsoncxx::builder::stream::document{};
	aqiDoc << "timestamp" << timeStamp << "aqi" << aqi;
	collection.insert_one(aqiDoc.view());

	// Log to the console window.
	cout << " TimeStamp: " << timeStamp << " AQI: " << aqi << endl;
}


int main()
{
	// Get the required parameters from environment variable.
	auto mongoURIStr = getEnvironmentVariable("MONGODB_URI");
	auto apiToken = getEnvironmentVariable("AQICN_TOKEN");
	std::string city = "Delhi";
	static const mongocxx::uri mongoURI = mongocxx::uri{ mongoURIStr };

	if (mongoURI.to_string().empty() || apiToken.empty())
	{
		cout << "Invalid URI or API token. Please check the environment variables." << endl;
		return 0;
	}

	// Create an instance.
	mongocxx::instance inst{};
	mongocxx::options::client client_options;
	auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
	client_options.server_api_opts(api);
	mongocxx::client conn{ mongoURI, client_options };

	// Setup Database and Collection.
	const string dbName = "AQIMonitor";
	const string timeSeriesCollectionName = "AQIMonitorCollection";

	// Setup Time Series collection options.
	bsoncxx::builder::document timeSeriesCollectionOptions =
	{
	   "timeseries",
		{
		  "timeField", "timestamp",
		  "granularity", "minutes"
		}
	};

	auto aqiMonitorDB = conn[dbName];
	auto aqiMonitorCollection = aqiMonitorDB.has_collection(timeSeriesCollectionName)
		? aqiMonitorDB[timeSeriesCollectionName]
		: aqiMonitorDB.create_collection(timeSeriesCollectionName, timeSeriesCollectionOptions.view().get_document().value);

	// Fetch and update AQI every 30 minutes.
	while (true)
	{ 
		auto aqi = getAQI(city, apiToken);
		saveToCollection(aqiMonitorCollection, aqi);
		std::this_thread::sleep_for(std::chrono::minutes(30));
	}

	return 0;
}
