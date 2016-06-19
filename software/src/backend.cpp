#include "network.hpp"
#include "../../firmware/packet.hpp"
#include <iostream>
#include <msl/serial.hpp>
#include <msl/time.hpp>
#include <stdexcept>
#include <sstream>
#include <string>

network_t network;
packet_cmd_t cmd;
packet_parser_t packet_parser;

bool network_read()
{
	network.poll();
	std::string recv_data(network.recv());
	bool handled=false;
	if(recv_data.size()>0)
		for(auto ii:recv_data)
			if(packet_parser.parse(ii))
				handled=(handled||packet_parser.recv_cmd(cmd));
	return handled;
}

void network_delay(const size_t millis)
{
	for(size_t ii=0;ii<millis/10;++ii)
	{
		network_read();
		msl::delay_ms(10);
	}
}

void print(const std::string& str)
{
	std::cout<<str<<std::endl;
	network.send(send_debug(str));
}

void print(std::ostringstream& ostr)
{
	print(ostr.str().c_str());
}

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
			print("Backend started.");
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
					network_delay(2000);
					if(!serial.good())
					{
						std::ostringstream ostr;
						ostr<<"Error opening serial on \""<<ii<<"\".";
						print(ostr);
					}
					else
					{
						std::ostringstream ostr;
						ostr<<"Serial opened on \""<<ii<<"\".";
						print(ostr);
						serial.write(send_cmd(cmd));
					}
					while(serial.good())
					{
						char temp;
						std::string serial_data;
						while(serial.available()>0&&serial.read(&temp,1)==1)
							serial_data+=temp;
						network.send(serial_data);
						if(network_read())
							serial.write(send_cmd(cmd));
						msl::delay_ms(10);
					}
					std::ostringstream ostr;
					print("Serial disconnected.");
					break;
				}
			}
			if(!found)
			{
				std::ostringstream ostr;
				print("Serial disconnected.");
			}
			network_delay(1000);
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