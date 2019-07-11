#pragma once
#include "websocket_helper.h"
#include <async_tcp/tcp_server.h>

class websocket_server : public websocket_helper
{
public:
	websocket_server(const char *ip, int port, boost::asio::io_service& io_service, message_factory* m_factory);
	virtual ~websocket_server();
	int websocket_message_handler(unsigned char* message, size_t connection);
	boost::asio::ip::tcp::acceptor m_client_acceptor;
	boost::asio::io_service& m_io_service;
	bool _is_mask;
	int _ping_interval;
	message_factory* _message_factory;
	void send_to_client(char * message_to_send);
	tcp_server *tcpserver;
};

