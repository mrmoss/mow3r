#include "network.hpp"
#include "../../firmware/packet.hpp"
#include <iostream>
#include <msl/serial.hpp>
#include <msl/time.hpp>
#include <stdexcept>

network_t network;
packet_cmd_t cmd{0,0,0};//FLAG_BLADE_ENABLE};
packet_parser_t packet_parser;

void network_read()
{
	network.poll();
	std::string recv_data(network.recv());
	packet_cmd_t cmd;
	std::string debug;
	if(recv_data.size()>0)
		for(auto ii:recv_data)
		{
			if(packet_parser.parse(ii))
			{
				if(packet_parser.recv_cmd(cmd))
					continue;
				if(packet_parser.recv_debug(debug))
					std::cout<<"Network: "<<debug<<std::endl;
			}
		}
}

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
			network_read();
			cmd.L=--cmd.R;
			if(cmd.L<-40)
			{
				cmd.L=cmd.R=0;
				/*if(cmd.flags&FLAG_BLADE_SPIN)
					cmd.flags=FLAG_BLADE_ENABLE;
				else
					cmd.flags=FLAG_BLADE_ENABLE|FLAG_BLADE_SPIN;*/
			}
			network.send(send_cmd(cmd));
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