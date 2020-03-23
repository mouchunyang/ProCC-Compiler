#include "SrcMain.h"

#include "cpprest/asyncrt_utils.h"
#include "cpprest/http_listener.h"
#include "cpprest/json.h"
#include "cpprest/uri.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

//function headers
std::vector<std::string> Split(const std::string& str, char delim);
void on_initialize(const string_t& address, utility::string_t filename);
void on_shutdown();

//class Headers
class MyServer;
struct Movie;

//Global Variable
std::unique_ptr<MyServer> g_server;

void ProcessCommandArgs(int argc, const char* argv[])
{
//https://github.com/microsoft/cpprestsdk/blob/master/Release/samples/BlackJack/BlackJack_Server/BlackJack_Server.cpp
	// TODO: Implement
    utility::string_t port = U("12345");
    utility::string_t fileName;
    if (argc == 2)
    {
        fileName = argv[1];
    }

    utility::string_t address = U("http://localhost:");
    address.append(port);

    on_initialize(address, fileName);
    std::cout << "Server is Opened" << std::endl;
    std::cout << "Press q to exit." << std::endl;

    std::string line;
    std::getline(std::cin, line);//change dont let it stop
    
    while (line != "q") {
        getline(std::cin, line);
    }

    on_shutdown();
    std::cout << "Server is Closed" << std::endl;
}

struct Movie{
    utility::string_t ID;
    int Runtime = 0;
    utility::string_t Title;
    utility::string_t URL;
    int Year = 0;
    std::vector<utility::string_t> Genres;
};

class MyServer
{
public:
    MyServer() = default;
    MyServer(utility::string_t url, utility::string_t filename);

    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }

private:
    void handle_get(http_request message);
    std::map<utility::string_t, Movie*> mIDMap;
    //std::unordered_map<utility::string_t, Movie*> mTitleMap;
    http_listener m_listener;
};


MyServer::MyServer(utility::string_t url, utility::string_t filename) : m_listener(url){
    
    m_listener.support(methods::GET, std::bind(&MyServer::handle_get, this, std::placeholders::_1));
    
    std::ifstream ifile(filename);
    if (ifile.is_open()){
        std::string line;
        while (!ifile.eof()) {
            getline(ifile, line);
            size_t typeLoc = line.find_first_of('\t', 0) + 1;
            if (line.substr(typeLoc, 5) == "movie"){
                std::vector<utility::string_t> inputs = Split(line, '\t');
                Movie* movie = new Movie();
                movie->ID = inputs[0];
                movie->Title = inputs[2];
                movie->Year = (inputs[5] == "\\N") ? 0 : stoi(inputs[5]);
                movie->Runtime = (inputs[7] == "\\N") ? 0 : stoi(inputs[7]);
                movie->URL = "https://www.imdb.com/title/" + movie->ID + "/";
                
                movie->Genres = Split(inputs[8], ',');
                
                mIDMap[movie->ID] = movie;
            }
        }
    }else{
        std::cout << "No file Input" << std::endl;
    }
}


void on_initialize(const string_t& address, utility::string_t filename)
{
    // Build our listener's URI from the configured address and the hard-coded path "blackjack/dealer"
    //Referenced from
//https://github.com/microsoft/cpprestsdk/blob/master/Release/samples/BlackJack/BlackJack_Server/BlackJack_Server.cpp
    
    uri_builder uri(address);
    uri.append_path(U("movie"));

    auto addr = uri.to_uri().to_string();
    
    g_server = std::make_unique<MyServer>(addr, filename);
    g_server->open().wait();

    ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;
}

void on_shutdown()
{
//https://github.com/microsoft/cpprestsdk/blob/master/Release/samples/BlackJack/BlackJack_Server/BlackJack_Server.cpp
    g_server->close().wait();
}

void MyServer::handle_get(http_request message)
{
    ucout << message.to_string() << '\n';
    
    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    //debug
//    for (int i = 0; i < paths.size(); ++i){
//        std::cout << paths[i] << ' ';
//    }
//    std::cout << std::endl;
    
    if (paths.empty())
    {
        auto response = json::value::object();
        response["Error"] = json::value::string("No movie with that ID found");
        message.reply(status_codes::NotFound, response);
        return;
    }

    // Get information on a specific table.
    if (paths[0] == "id"){
        auto found = mIDMap.find(paths.back());
        if (found == mIDMap.end())
        {
            auto response = json::value::object();
            response["Error"] = json::value::string("No movie with that ID found");
            message.reply(status_codes::NotFound, response);
            return;
        }
       
        Movie* movie = found->second;
        auto response = json::value::object();
        
        auto genresArr = json::value::array();
        for (int i = 0; i < movie->Genres.size(); ++i){
            genresArr[i] = json::value::string(movie->Genres[i]);
        }
        response["Genres"] = genresArr;
        response["ID"] = json::value::string(movie->ID);
        response["Runtime"] = movie->Runtime;
        response["Title"] = json::value::string(movie->Title);
        response["URL"] = json::value::string(movie->URL);
        response["Year"] = movie->Year;
        message.reply(status_codes::OK, response);
    
    }else{
        //Simply use linear search
        utility::string_t targetTitle = paths.back();
        auto responseArr = json::value::array();
        int arrIndex = 0;
        for (auto it = mIDMap.begin(); it != mIDMap.end(); ++it){
            if (it->second->Title == targetTitle){
                Movie* movie = it->second;
                auto response = json::value::object();
                
                auto genresArr = json::value::array();
                for (int i = 0; i < movie->Genres.size(); ++i){
                    genresArr[i] = json::value::string(movie->Genres[i]);
                }
                response["Genres"] = genresArr;
                response["ID"] = json::value::string(movie->ID);
                response["Runtime"] = movie->Runtime;
                response["Title"] = json::value::string(movie->Title);
                response["URL"] = json::value::string(movie->URL);
                response["Year"] = movie->Year;
                
                responseArr[arrIndex++] = response;
            }
        }
        if (responseArr.size() > 0){
            message.reply(status_codes::OK, responseArr);
        }
        else{
            auto response = json::value::object();
            response["Error"] = json::value::string("No movie by that name found");
            message.reply(status_codes::NotFound, response);
        }
    }
};

//Referenced from ITP380 Lab5 CSVHelper.h
std::vector<utility::string_t> Split(const utility::string_t& str, char delim)
{
    //const char delim = '\t';
    std::vector<std::string> retVal;
    
    size_t start = 0;
    size_t delimLoc = str.find_first_of(delim, start);
    while (delimLoc != utility::string_t::npos)
    {
        retVal.emplace_back(str.substr(start, delimLoc - start));
        
        start = delimLoc + 1;
        delimLoc = str.find_first_of(delim, start);
    }
    
    retVal.emplace_back(str.substr(start));
    return retVal;
}

