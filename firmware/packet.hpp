#ifndef PACKET_HPP
#define PACKET_HPP

#ifdef __AVR
	#include <Arduino.h>
#else
	#include <cstdint>
	#include <sstream>
	#include <string>
#endif

#define PACKET_HEADER0		0xf0
#define PACKET_HEADER1		0x0f
#define PACKET_NONE			0x00
#define PACKET_CMD			0x01
#define PACKET_DEBUG		0x02

#define FLAG_BLADE_ENABLE	0x01
#define FLAG_BLADE_SPIN		0x02

struct packet_cmd_t
{
	int16_t L;
	int16_t R;
	uint16_t flags;
};

uint8_t calc_crc(const uint8_t* buf,const uint16_t len)
{
	uint8_t crc=0;
	for(uint16_t ii=0;ii<len;++ii)
		crc^=buf[ii];
	return crc;
}

#ifndef __AVR
std::string send_cmd(const packet_cmd_t& cmd)
{
	std::string str;
	str+=(uint8_t)PACKET_HEADER0;
	str+=(uint8_t)PACKET_HEADER1;
	str+=(uint8_t)PACKET_CMD;
	str+=(uint8_t)sizeof(packet_cmd_t);
	str+=std::string((char*)&cmd,sizeof(packet_cmd_t));
	str+=calc_crc((uint8_t*)str.c_str(),str.size());
	return str;
}

std::string send_debug(std::string message)
{
	std::string str;
	while(message.size()>0)
	{
		std::string sub_message=message.substr(0,255);
		str+=(uint8_t)PACKET_HEADER0;
		str+=(uint8_t)PACKET_HEADER1;
		str+=(uint8_t)PACKET_DEBUG;
		str+=(uint8_t)sub_message.size();
		str+=sub_message;
		str+=calc_crc((uint8_t*)str.c_str(),str.size());
		message=message.substr(sub_message.size(),
			message.size()-sub_message.size());
	}
	return str;
}
#endif

class packet_parser_t
{
	public:
		enum state_t
		{
			HEADER0,
			HEADER1,
			TYPE,
			SIZE,
			DATA,
			CRC
		};

		packet_parser_t():state_m(HEADER0),size_m(0)
		{
			data_m[2]=PACKET_NONE;
		}

		bool parse(const uint8_t b)
		{
			if(state_m==HEADER0&&b==PACKET_HEADER0)
			{
				data_m[0]=b;
				state_m=HEADER1;
			}
			else if(state_m==HEADER1&&b==PACKET_HEADER1)
			{
				data_m[1]=b;
				state_m=TYPE;
			}
			else if(state_m==TYPE)
			{
				data_m[2]=b;
				state_m=SIZE;
			}
			else if(state_m==SIZE)
			{
				size_m=b;
				data_m[3]=0;
				state_m=DATA;
			}
			else if(state_m==DATA)
			{
				data_m[4+(data_m[3]++)]=b;
				if(data_m[3]>=size_m)
					state_m=CRC;
			}
			else if(state_m==CRC)
			{
				data_m[4+size_m]=b;
				state_m=HEADER0;
				return (calc_crc(data_m,4+size_m+1)==0);
			}
			else
			{
				state_m=HEADER0;
			}
			return false;
		}

		bool recv_cmd(packet_cmd_t& cmd)
		{
			if(state_m==HEADER0&&data_m[0]==PACKET_HEADER0&&
				data_m[1]==PACKET_HEADER1&&data_m[2]==PACKET_CMD)
			{
				data_m[1]=PACKET_NONE;
				for(uint16_t ii=0;ii<sizeof(packet_cmd_t);++ii)
					((uint8_t*)&cmd)[ii]=data_m[4+ii];
				return true;
			}
			return false;
		}

		#ifndef __AVR
		bool recv_debug(std::string& debug)
		{
			if(state_m==HEADER0&&data_m[0]==PACKET_HEADER0&&
				data_m[1]==PACKET_HEADER1&&data_m[2]==PACKET_DEBUG)
			{
				data_m[1]=PACKET_NONE;
				debug=std::string((char*)(data_m+4),data_m[3]);
				return true;
			}
			return false;
		}
		#endif
	private:
		state_t state_m;
		uint16_t size_m;
		uint8_t data_m[4+255+1];
};

#endif
