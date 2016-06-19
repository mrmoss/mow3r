#include "network.hpp"
#include <msl/crypto.hpp>
#include <iomanip>
#include <sstream>

static std::string to_hex_string(const std::string& str)
{
	std::ostringstream ostr;
	for(auto ii:str)
		ostr<<std::hex<<std::setw(2)<<std::setfill('0')<<((int)ii&0x000000ff);
	return ostr.str();
}

void network_t_tx_handler(mg_connection* conn,int ev,void* p)
{
	network_t& network(*(network_t*)(conn->mgr->user_data));
	for(auto data:network.tx_data_m)
	{
		data=to_hex_string(msl::hmac_sha256(network.auth_m,data))+data;
		mg_send(conn,data.c_str(),data.size());
	}
	network.tx_data_m.clear();
}

void network_t_rx_handler(mg_connection* conn,int ev,void* p)
{
	if(ev==MG_EV_RECV)
	{
		mbuf* io=&conn->recv_mbuf;
		network_t& network(*(network_t*)(conn->mgr->user_data));
		bool auth=false;
		std::string data(io->buf,io->len);
		if(data.size()>=64)
		{
			std::string hash=data.substr(0,64);
			data=data.substr(64,data.size()-64);
			auth=(to_hex_string(msl::hmac_sha256(network.auth_m,data))==hash);
		}
		network.recv_cb_m(data,auth);
		mbuf_remove(io,io->len);
		conn->flags|=MG_F_SEND_AND_CLOSE;
	}
}

network_t::network_t(const std::string& tx,const std::string& rx,
	const std::string& auth,recv_cb_t recv_cb):
	tx_m(tx),rx_m(rx),auth_m(auth),recv_cb_m(recv_cb)
{}

network_t::status_t network_t::init()
{
	mg_mgr_init(&mgr_m,this);
	if(mg_connect(&mgr_m,tx_m.c_str(),network_t_tx_handler)==nullptr)
		return BAD_CONNECT;
	if(mg_bind(&mgr_m,rx_m.c_str(),network_t_rx_handler)==nullptr)
		return BAD_BIND;
	return SUCCESS;
}

void network_t::poll()
{
	mg_mgr_poll(&mgr_m,10);
}

void network_t::free()
{
	mg_mgr_free(&mgr_m);
}

void network_t::send(const void* buf,const size_t len)
{
	send(std::string((char*)buf,len));
}

void network_t::send(const std::string& buf)
{
	tx_data_m.push_back(buf);
}