#include "../../firmware/packet.h"
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
size_t last_print_times=0;
std::string last_print;

void smart_print(const std::string& str,bool ignore_times=false)
{
	if(last_print!=str)
	{
		last_print_times=0;
		last_print=str;
		std::cout<<std::endl;
	}
	std::cout<<"\r"<<str<<std::flush;
	if(!ignore_times&&last_print_times>0)
		std::cout<<" ("<<last_print_times<<")"<<std::flush;
	++last_print_times;
}

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
	smart_print(str);
}

void network_update()
{
	network.poll();
	if(msl::millis()>deadman_timer&&!is_stopped())
	{
		stop();
		deadman_timer=msl::millis()+deadman_timeout;
		smart_print("Stopping.");
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
			smart_print("Invalid serial object \""+data+"\".");
		}
		smart_print("Pilot connected.");
	}
	else
	{
		smart_print("Authentication mismatch for \""+data+"\".");
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
		smart_print("Frontend started.");
		smart_print("Pin: "+pin);
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
						smart_print("Error opening serial on \""+ii+"\".");
					}
					else
					{
						smart_print("Serial opened on \""+ii+"\".");
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
									smart_print("Debug: "+debug);
								if(packet_parser.recv_heartbeat())
									got_serial_heartbeat=true;
							}
						network_update();
						serial.write(send_cmd(cmd));
						msl::delay_ms(10);
					}
					smart_print("Serial disconnected.");
					break;
				}
			}
			if(!found)
				smart_print("Serial disconnected.");
			network_delay(1000);
		}
		network.free();
	}
	catch(std::exception& error)
	{
		smart_print(error.what());
		return 1;
	}
	catch(...)
	{
		smart_print("Unexpected error.");
		return 1;
	}
	return 0;
}
