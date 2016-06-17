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
		network_t::status_t status(network.init("udp://:8080","udp://:8081"));
		if(status==network_t::BAD_CONNECT)
			throw std::runtime_error("Error creating outbound backend connection.");
		if(status==network_t::BAD_BIND)
			throw std::runtime_error("Error creating inbound backend connection.");
		std::cout<<" Backend started."<<std::endl;
		while(true)
		{
			auto serials=msl::serial_t::list();
			for(auto ii:serials)
			{
				if(ii.find("Bluetooth")==std::string::npos)
				{
					msl::serial_t serial(ii,57600);
					serial.open();
					if(!serial.good())
						std::cout<<"Error opening serial on \""<<ii<<"\"."<<std::endl;
					else
						std::cout<<"Serial opened on \""<<ii<<"\"."<<std::endl;
					while(serial.good())
					{
						char temp;
						std::string serial_data;
						while(serial.available()>0&&serial.read(&temp,1)==1)
							serial_data+=temp;
						network.send(serial_data);
						network.poll();
					}
					std::cout<<"Serial disconnected."<<std::endl;
				}
			}
			msl::delay_ms(10);
			network.poll();
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