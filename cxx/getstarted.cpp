#include <iostream>
#include <cstdint>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

int main(int, char**) {

    const char* mongoURI = getenv("MONGODB_URI");
    mongocxx::instance inst{};
    mongocxx::uri uri(mongoURI);

    mongocxx::client conn{uri};

    auto collection = conn["getstarted"]["cxx"];

    // Create a document
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = builder
        << "item" << "journal"
        << "qty" << 25
        << "tags" << bsoncxx::builder::stream::open_array
            << "blank" << "red"
        << close_array
        << "size" << bsoncxx::builder::stream::open_document
            << "h" << 14
            << "w" << 21
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::finalize;

    // Insert a single document 
    bsoncxx::document::view doc_view = doc_value.view();
    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(doc_view);

    // Insert another document (alternative way) 
    builder << "field_name" << "value_1";
    collection.insert_one(builder.view());

    // Find one with a query filter 
    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = collection.find_one(document{} << "i" << 102 << finalize);
    if(maybe_result) {
        std::cout << bsoncxx::to_json(*maybe_result) << "\n";
    }

    // Find all in a collection 
    auto cursor = collection.find({});
    for (auto&& doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }

    bsoncxx::stdx::optional<mongocxx::result::delete_result> delete_result = collection.delete_many({});
    if(delete_result) {
        std::cout <<"Number of deleted document(s): "<< delete_result->deleted_count() << "\n";
    }

    std::cout << "Completed\n";
}