#include "../../firmware/packet.hpp"
#include "network.hpp"
#include <iostream>
#include <msl/serial.hpp>
#include <msl/time.hpp>
#include <stdexcept>

network_t network;
cmd_t cmd{0,0};

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
			std::cout<<data<<std::flush;
			cmd.L++;
			if(cmd.L>255)
			{
				cmd.R+=1;
				cmd.L=0;
			}
			if(cmd.R>255)
				cmd.R=0;
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