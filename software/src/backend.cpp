#include "network.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <msl/time.hpp>

void recv_cb(std::string data,bool auth);
network_t network("udp://224.0.0.1:8081","udp://:8080","auth123",recv_cb);

void recv_cb(std::string data,bool auth)
{
	if(auth)
	{
		std::cout<<"Data: "<<data<<std::endl;
		network.send("GOOD!");
	}
	else
	{
		std::cout<<"Authentication mismatch."<<std::endl;
		network.send("Bad!");
	}
}

int main()
{
	try
	{
		network_t::status_t status(network.init());
		if(status==network_t::BAD_CONNECT)
			throw std::runtime_error("Error creating outbound frontend connection.");
		if(status==network_t::BAD_BIND)
			throw std::runtime_error("Error creating inbound frontend connection.");
		std::cout<<"Frontend started."<<std::endl;
		while(true)
			network.poll();
		network.free();
	}
	catch(std::exception& error)
	{
		std::cout<<error.what()<<std::endl;
		return 1;
	}
	catch(...)
	{
		std::cout<<"Unexpected error."<<std::endl;
		return 1;
	}
	return 0;
}