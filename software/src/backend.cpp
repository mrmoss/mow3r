#include "network.hpp"
#include <iostream>
#include <msl/serial.hpp>
#include <msl/time.hpp>
#include <stdexcept>
#include <sstream>
#include <string>

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
		{
			std::ostringstream ostr;
			ostr<<"Backend started."<<std::endl;
			std::cout<<ostr.str()<<std::flush;
			network.send(ostr.str());
		}
		while(true)
		{
			auto serials=msl::serial_t::list();
			bool found=false;
			for(auto ii:serials)
			{
				if(ii.find("Bluetooth")==std::string::npos)
				{
					found=true;
					msl::serial_t serial(ii,115200);
					serial.open();
					msl::delay_ms(2000);
					if(!serial.good())
					{
						std::ostringstream ostr;
						ostr<<"Error opening serial on \""<<ii<<"\"."<<std::endl;
						std::cout<<ostr.str()<<std::flush;
						network.send(ostr.str());
					}
					else
					{
						std::ostringstream ostr;
						ostr<<"Serial opened on \""<<ii<<"\"."<<std::endl;
						std::cout<<ostr.str()<<std::flush;
						network.send(ostr.str());
					}
					while(serial.good())
					{
						char temp;
						std::string serial_data;
						while(serial.available()>0&&serial.read(&temp,1)==1)
							serial_data+=temp;
						network.send(serial_data);
						network.poll();
						std::string recv_data(network.recv());
						if(recv_data.size()>0)
							serial.write(recv_data);
					}
					std::ostringstream ostr;
					ostr<<"Serial disconnected."<<std::endl;
					std::cout<<ostr.str()<<std::flush;
					network.send(ostr.str());
					break;
				}
			}
			if(!found)
			{
				std::ostringstream ostr;
				ostr<<"No suitable serial ports found."<<std::endl;
				std::cout<<ostr.str()<<std::flush;
				network.send(ostr.str());
			}
			for(size_t ii=0;ii<100;++ii)
			{
				network.poll();
				msl::delay_ms(10);
			}
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