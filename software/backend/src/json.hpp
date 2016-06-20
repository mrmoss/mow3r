#ifndef JSON_HPP
#define JSON_HPP

#include <jsoncpp/json.h>
#include <string>


typedef Json::Value json_t;
std::string serialize(const json_t& json);
json_t deserialize(const std::string& str);

#endif