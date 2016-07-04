#include "../../firmware/packet.hpp"
#include "json.hpp"
#include "network.hpp"
#include <cstdint>
#include <iostream>
#include <msl/crypto.hpp>
#include <msl/serial.hpp>
#include <msl/string.hpp>
#include <msl/time.hpp>
#include <stdexcept>
#include <string>

void recv_cb(std::string data,bool auth);
std::string pin=msl::to_hex_string(
	msl::hash_sha256(msl::crypto_rand(8))).substr(0,8);
network_t network("udp://224.0.0.1:8081","udp://:8080",pin,recv_cb);
packet_cmd_t cmd;
packet_parser_t packet_parser;
std::int64_t deadman_timeout=1000;
std::int64_t deadman_timer=msl::millis()+deadman_timeout;
std::int64_t heartbeat_timeout=100;
std::int64_t heartbeat_timer=msl::millis()+heartbeat_timeout;
json_t status;
bool got_serial_heartbeat=false;

void stop()
{
	cmd.L=cmd.R=cmd.flags=0;
}

bool is_stopped()
{
	return (cmd.L==0&&cmd.R==0&&cmd.flags==0);
}

void network_print(const std::string& str)
{
	json_t json;
	json["debug"]=str;
	network.send(serialize(json));
	std::cout<<str<<std::endl;
}

void network_update()
{
	network.poll();
	if(msl::millis()>deadman_timer&&!is_stopped())
	{
		stop();
		deadman_timer=msl::millis()+deadman_timeout;
		std::cout<<"Stopping."<<std::endl;
	}
	if(msl::millis()>=heartbeat_timer)
	{
		status["heartbeat"]=status["heartbeat"].asInt()+1;
		if(status["heartbeat"].asInt()>255)
			status["heartbeat"]=0;
		if(got_serial_heartbeat)
		{
			status["ardubeat"]=status["ardubeat"].asInt()+1;
			got_serial_heartbeat=false;
		}
		if(status["ardubeat"].asInt()>255)
			status["ardubeat"]=0;
		heartbeat_timer=msl::millis()+heartbeat_timeout;
		network.send(serialize(status));
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
			cmd.flags=json["flags"].asInt();
			deadman_timer=msl::millis()+deadman_timeout;
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
		status["heartbeat"]=0;
		status["ardubeat"]=0;
		stop();
		network_t::status_t status(network.init());
		if(status==network_t::BAD_CONNECT)
			throw std::runtime_error("Error creating outbound frontend connection.");
		if(status==network_t::BAD_BIND)
			throw std::runtime_error("Error creating inbound frontend connection.");
		std::cout<<"Frontend started."<<std::endl;
		std::cout<<"Pin: "<<pin<<std::endl;
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
						std::cout<<"Error opening serial on \""+ii+"\"."<<std::endl;
					}
					else
					{
						std::cout<<"Serial opened on \""+ii+"\"."<<std::endl;
						serial.write(send_cmd(cmd));
					}
					while(serial.good())
					{
						char temp;
						while(serial.available()>0&&serial.read(&temp,1)==1)
							if(packet_parser.parse(temp))
							{
								std::string debug;
								if(packet_parser.recv_debug(debug))
									std::cout<<"Debug: "+debug<<std::endl;
								if(packet_parser.recv_heartbeat())
									got_serial_heartbeat=true;
							}
						network_update();
						serial.write(send_cmd(cmd));
						msl::delay_ms(10);
					}
					std::cout<<"Serial disconnected."<<std::endl;
					break;
				}
			}
			if(!found)
				std::cout<<"Serial disconnected."<<std::endl;
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
