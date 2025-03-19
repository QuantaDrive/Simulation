//
// Created by dante on 2/14/25.
//

#include "PhysicalManager.h"

#include <fstream>
#include <curlpp/cURLpp.hpp>
#include "curlpp/Easy.hpp"
#include "curlpp/Options.hpp"
#include <cstring>

template<typename T, typename U>
T safer_cast(const U& from) {
    T to;
    memcpy(&to, &from, (sizeof(T) > sizeof(U) ? sizeof(U) : sizeof(T)));
    return to;
}

void PhysicalManager::executeTask(const std::string& filename)
{
    //TODO: webserver op arm
    curlpp::Easy request;

    request.setOpt(new curlpp::options::Url("localhost:8000/gcode"));
    request.setOpt(new curlpp::options::Verbose(true));

    std::list<std::string> header;
    header.emplace_back("Content-Type: application/octet-stream");

    request.setOpt(new curlpp::options::HttpHeader(header));

    std::string content;
    std::string line;
    ifstream in;
    in.open(filename);
    while (getline(in, line))
    {
        content += line + "\n";
    }
    cout << content << endl;


    request.setOpt(new curlpp::options::PostFields(content));
    request.setOpt(new curlpp::options::PostFieldSize(safer_cast<unsigned long,long>(content.size()+1)));

    request.perform();
}
