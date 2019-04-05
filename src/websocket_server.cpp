#include "websocket_server/websocket_server.h"

websocket_server::websocket_server(char *ip, int port, boost::asio::io_service& io_service, message_factory* message_factory) : websocket_helper(),
m_io_service(io_service),
m_client_acceptor(io_service)
{
	_is_mask = false;
	_ping_interval = 50;
	_message_factory = message_factory;

	//message_factory->on_receive.connect(boost::bind(&websocket_server::websocket_message_handler, this, _1));
	message_factory->on_receive.connect(boost::bind(&websocket_server::websocket_message_handler, this, _1, _2));
	try {
		tcpserver = new tcp_server(ip, port, m_io_service, message_factory);
		boost::thread t(boost::bind(static_cast<size_t(boost::asio::io_service::*)()>(&boost::asio::io_service::run), &m_io_service));
		tcpserver->run();
		t.join();
	}
	catch (boost::system::system_error& e)
	{
	}
}

websocket_server::~websocket_server()
{
}

int websocket_server::websocket_message_handler(unsigned char *message, size_t connection)
{
	if (message[0] != 129)
	{
	//	tcpserver->broadcast(create_handshake_message(message));
		_message_factory->write_message(create_handshake_message(message), connection);
		DEBUG_CONSOLE(message);
		return 0;
	} else
	{
		unsigned char data[1024];
		
		int len = parse_masked_data(key, message, &data[0]);
		//unsigned char *temp = new unsigned char[len];
		//un_mask_data(key,&data[0],len, &data[0]);
		DEBUG_CONSOLE(data);
	}
	return 0;
}
void websocket_server::send_to_client(char * message_to_send)
{
	std::string m_tx_buffer;
	m_tx_buffer.append(message_to_send, strlen(message_to_send));
	unsigned char* buffer_to_send = 0;
	websocket_framing(m_tx_buffer, buffer_to_send);

	//_message_factory->on_send(buffer_to_send);
	
	m_tx_buffer.clear();
}