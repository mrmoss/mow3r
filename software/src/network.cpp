#include "network.hpp"

void network_t_tx_handler(mg_connection* conn,int ev,void* p)
{
	network_t& network(*(network_t*)(conn->mgr->user_data));
	if(network.tx_data_m.size()>0)
	{
		mg_send(conn,network.tx_data_m.c_str(),network.tx_data_m.size());
		network.tx_data_m="";
	}
}

void network_t_rx_handler(mg_connection* conn,int ev,void* p)
{
	if(ev==MG_EV_RECV)
	{
		mbuf* io=&conn->recv_mbuf;
		network_t& network(*(network_t*)(conn->mgr->user_data));
		network.rx_data_m+=std::string(io->buf,io->len);
		mbuf_remove(io,io->len);
		conn->flags|=MG_F_SEND_AND_CLOSE;
	}
}

network_t::network_t()
{}

network_t::status_t network_t::init(const std::string& tx,const std::string& rx)
{
	mg_mgr_init(&mgr_m,this);
	if(mg_connect(&mgr_m,tx.c_str(),network_t_tx_handler)==nullptr)
		return BAD_CONNECT;
	if(mg_bind(&mgr_m,rx.c_str(),network_t_rx_handler)==nullptr)
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
	tx_data_m+=std::string((char*)buf,len);
}

void network_t::send(const std::string& buf)
{
	tx_data_m+=buf;
}

std::string network_t::recv()
{
	std::string data=rx_data_m;
	rx_data_m="";
	return data;
}