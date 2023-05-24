#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;


// Get all the databases from a given client.
vector<string> getDatabases(mongocxx::client& client)
{
	return client.list_database_names();
}


int main()
{
    auto mongoURIStr = "";
    static const mongocxx::uri mongoURI = mongocxx::uri{ mongoURIStr };

    // Create an instance.
    mongocxx::instance inst{};
    mongocxx::options::client client_options;
    auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
    client_options.server_api_opts(api);
    mongocxx::client conn{ mongoURI, client_options};

    // Get the databases.
     auto dbs = getDatabases(conn);
     for (auto db : dbs)
     {
         cout << db << endl;
    }

    return 0;
}