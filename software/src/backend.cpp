#include "../../firmware/packet.hpp"
#include "json.hpp"
#include "network.hpp"
#include <cstdint>
#include <iostream>
#include <msl/serial.hpp>
#include <msl/time.hpp>
#include <stdexcept>
#include <string>

void recv_cb(std::string data,bool auth);
network_t network("udp://224.0.0.1:8081","udp://:8080","auth123",recv_cb);
packet_cmd_t cmd;
packet_parser_t packet_parser;
std::int64_t deadman_timeout=1000;
std::int64_t deadman_timer=msl::millis()+deadman_timeout;

void stop()
{
	cmd.L=cmd.R=cmd.flags=0;
}

bool is_stopped()
{
	return (cmd.L==cmd.R==cmd.flags==0);
}

void network_print(const std::string& str)
{
	network.send(str);
	std::cout<<str<<std::endl;
}

void network_update()
{
	network.poll();
	if(msl::millis()>deadman_timer&&!is_stopped())
	{
		stop();
		deadman_timer=msl::millis()+deadman_timeout;
		network_print("Stopping.");
	}
}

void network_delay(const size_t millis)
{
	for(size_t ii=0;ii<millis/10;++ii)
		network_update();
}

void recv_cb(std::string data,bool auth)
{
	if(auth)
	{
		try
		{
			json_t json(deserialize(data));
			cmd.L=json["L"].asInt();
			cmd.R=json["R"].asInt();
			deadman_timer=msl::millis()+deadman_timeout;
			std::cout<<"Data: "<<cmd.L<<","<<cmd.R<<std::endl;
		}
		catch(...)
		{
			std::cout<<"Invalid serial object \""<<data<<"\"."<<std::endl;
		}
	}
	else
	{
		std::cout<<"Authentication mismatch for \""<<data<<"\"."<<std::endl;
	}
}

int main()
{
	try
	{
		stop();
		network_t::status_t status(network.init());
		if(status==network_t::BAD_CONNECT)
			throw std::runtime_error("Error creating outbound frontend connection.");
		if(status==network_t::BAD_BIND)
			throw std::runtime_error("Error creating inbound frontend connection.");
		network_print("Frontend started.");
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
						network_print("Error opening serial on \""+ii+"\".");
					}
					else
					{
						network_print("Serial opened on \""+ii+"\".");
						serial.write(send_cmd(cmd));
					}
					while(serial.good())
					{
						network_update();
						char temp;
						while(serial.available()>0&&serial.read(&temp,1)==1)
							if(packet_parser.parse(temp))
							{
								std::string debug;
								if(packet_parser.recv_debug(debug))
									network_print("Debug: "+debug);
							}
						serial.write(send_cmd(cmd));
						msl::delay_ms(10);
					}
					network_print("Serial disconnected.");
					break;
				}
			}
			if(!found)
				network_print("Serial disconnected.");
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