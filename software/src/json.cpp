#include "json.hpp"

#include <sstream>

std::string serialize(const json_t& json)
{
	Json::FastWriter writer;
	std::string ret=writer.write(json);
	while(ret.size()>0&&isspace(ret[ret.size()-1])!=0)
		ret.pop_back();
	return ret;
}

json_t deserialize(const std::string& str)
{
	std::istringstream istr(str);
	json_t json;
	istr>>json;
	return json;
}