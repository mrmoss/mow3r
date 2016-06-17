#include "network.hpp"
#include <iostream>
#include <msl/serial.hpp>
#include <msl/time.hpp>
#include <stdexcept>

network_t network;

int main()
{
	try
	{
		network_t::status_t status(network.init("udp://:8081","udp://:8080"));
		if(status==network_t::BAD_CONNECT)
			throw std::runtime_error("Error creating outbound frontend connection.");
		if(status==network_t::BAD_BIND)
			throw std::runtime_error("Error creating inbound frontend connection.");
		std::cout<<"Frontend started."<<std::endl;
		while(true)
		{
			network.poll();
			auto data=network.recv();
			if(data.size()>0)
				std::cout<<"RX: "<<data.size()<<std::endl;
			msl::delay_ms(10);
		}
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