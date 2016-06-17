#include <mongoose/mongoose.h>
#include <string>

class network_t
{
	public:
		enum status_t
		{
			SUCCESS,
			BAD_CONNECT,
			BAD_BIND
		};
		network_t();
		network_t(const network_t& copy)=delete;
		network_t& operator=(const network_t& copy)=delete;
		status_t init(const std::string& tx_addr,const std::string& rx_addr);
		void poll();
		void free();
		void send(const void* buf,const size_t len);
		void send(const std::string& buf);
		std::string recv();
	private:
		friend void network_t_tx_handler(mg_connection* conn,int ev,void* p);
		friend void network_t_rx_handler(mg_connection* conn,int ev,void* p);
		mg_mgr mgr_m;
		const std::string tx_m;
		const std::string rx_m;
		std::string tx_data_m;
		std::string rx_data_m;
};

void network_t_tx_handler(mg_connection* conn,int ev,void* p);
void network_t_rx_handler(mg_connection* conn,int ev,void* p);
