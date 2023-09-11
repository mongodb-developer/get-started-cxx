#include <mongocxx/client.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#define FILE_NAME "name"
#define FILE_PATH "path"
#define FILE_DATA "data"

using bsoncxx::builder::basic::kvp; 
using bsoncxx::builder::basic::make_document;

// ********************************************** Helper Functions **********************************************

// Upload a file to the collection.
bool upload(const std::string& filePath, mongocxx::collection& collection)
{
    // Open the binary file
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) 
    {
        std::cout << "Failed to open the file: " << filePath << std::endl;
        return false;
    }

    // Get the file size.
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the file content into a buffer
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) 
    {
        std::cout << "Failed to read the file: " << filePath << std::endl;
        return false;
    }

    // Create the binary object for bsoncxx.
    bsoncxx::types::b_binary data{bsoncxx::binary_sub_type::k_binary, static_cast<std::uint32_t>(fileSize), reinterpret_cast<const std::uint8_t*>(buffer.data())};

    // Create a document with the file name and file content.

    auto doc = make_document(
	        kvp(FILE_NAME, std::filesystem::path(filePath).filename()),
	        kvp(FILE_PATH, filePath),
	        kvp(FILE_DATA, data));

    // Insert the document into the collection.
    auto result = collection.insert_one(doc.view());
    
    if(result)
    {
        std::cout << "Upload successful for: " << filePath << std::endl;
    }
    else
    {
        std::cout << "Upload failed for: " << filePath << std::endl;
    }
    return true;
}

// Download a file from a collection to a given folder.
bool download(const std::string& fileName, const std::string& destinationFolder, mongocxx::collection& collection)
{
    // Create a query to find the file by filename
    auto filter = make_document(kvp(FILE_NAME, fileName));

     // Find the document in the collection
    auto result = collection.find_one(filter.view());

    if (result) 
    {
        // Get the binary data from the document
        bsoncxx::document::view binaryDocView = result->view();
        auto binaryData = binaryDocView[FILE_DATA].get_binary();
        
        // Create a file to save the binary data
        std::ofstream file(destinationFolder + fileName, std::ios::binary);
        if (!file) 
        {
            std::cout << "Failed to create the file: " << fileName << " at " << destinationFolder << std::endl;
            return false;
        }
        
        // Write the binary data to the file
        file.write(reinterpret_cast<const char*>(binaryData.bytes), binaryData.size);

        std::cout << "Download successful for: " << fileName << " at " << destinationFolder << std::endl;
        return true;
    } 
    else 
    {
        std::cout << "File not found in the collection: " << fileName << std::endl;
        return false;
    }
}


// ********************************************** Main **********************************************
int main()
{
    try
    {
        auto mongoURIStr = "<Insert MongoDB Connection String>";
        static const mongocxx::uri mongoURI = mongocxx::uri{ mongoURIStr };
        
        // Create an instance.
        mongocxx::instance inst{};
        
        mongocxx::options::client client_options;
        auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
        client_options.server_api_opts(api);
        mongocxx::client conn{ mongoURI, client_options};
        
        const std::string dbName = "fileStorage";
        const std::string collName = "files";
        
        auto fileStorageDB = conn.database(dbName);
        auto filesCollection = fileStorageDB.collection(collName);
        // Drop previous data.
        filesCollection.drop();

        // Upload all files in the upload folder.
        const std::string uploadFolder = "/Users/bishtr/repos/fileStorage/upload/";
        for (const auto & filePath : std::filesystem::directory_iterator(uploadFolder))
        {
            if(std::filesystem::is_directory(filePath))
                continue;

            if(!upload(filePath.path().string(), filesCollection))
            {
                std::cout << "Upload failed for: " << filePath.path().string() << std::endl;
            }
        }

        // Download files to the download folder.
        const std::string downloadFolder = "/Users/bishtr/repos/fileStorage/download/";
        
        // Search with specific filenames and download it.
        const std::string fileName1 = "image-15.jpg", fileName2 = "Hi Seed Shaker 120bpm On Accents.wav";
        for ( auto fileName : {fileName1, fileName2} )
        {
            if (!download(fileName, downloadFolder, filesCollection))
            {
                std::cout << "Download failed for: " << fileName << std::endl;
            } 
        }
        
        // Download all files in the collection.
        auto cursor = filesCollection.find({});
        for (auto&& doc : cursor) 
        {
            auto fileName = std::string(doc[FILE_NAME].get_string().value);
            if (!download(fileName, downloadFolder, filesCollection))
            {
                std::cout << "Download failed for: " << fileName << std::endl;
            } 
        }
    }
    catch(const std::exception& e)
    {
        std::cout << "Exception encountered: " << e.what() << std::endl;
    }

    return 0;
}