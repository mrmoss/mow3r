#include <mongoose/mongoose.h>
#include <functional>
#include <string>
#include <vector>

class network_t
{
	public:
		typedef std::function<void(std::string data,bool auth)> recv_cb_t;
		enum status_t
		{
			SUCCESS,
			BAD_CONNECT,
			BAD_BIND
		};
		network_t(const std::string& tx,const std::string& rx,
			const std::string& auth,recv_cb_t recv_cb);
		network_t(const network_t& copy)=delete;
		network_t& operator=(const network_t& copy)=delete;
		status_t init();
		void poll();
		void free();
		void send(const void* buf,const size_t len);
		void send(const std::string& buf);

	private:
		friend void network_t_tx_handler(mg_connection* conn,int ev,void* p);
		friend void network_t_rx_handler(mg_connection* conn,int ev,void* p);
		mg_mgr mgr_m;
		const std::string tx_m;
		const std::string rx_m;
		const std::string auth_m;
		recv_cb_t recv_cb_m;
		std::vector<std::string> tx_data_m;
};

void network_t_tx_handler(mg_connection* conn,int ev,void* p);
void network_t_rx_handler(mg_connection* conn,int ev,void* p);
