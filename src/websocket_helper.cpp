#include "websocket_server/websocket_helper.h"

websocket_helper::websocket_helper()
= default;


websocket_helper::~websocket_helper()
= default;

int websocket_helper::parse_masked_data(unsigned char * key, const unsigned char * data, unsigned char * actual_data)
{
	auto len = data[1] - 0x80;
	unsigned char mask_key[4];
	if (len<126) {
		for (auto i = 0; i<4; i++){
			key[i] = data[2 + i];
			mask_key[i] = key[i];
		}

		for (auto i = 0; i<len; i++)
			actual_data[i] = data[6 + i];
	}
	else if (len == 126) {
		unsigned int len_2 = 0;
		len_2 = data[2];
		len_2 = len_2 << 8;
		len_2 = len_2 + data[3];

		for (auto i = 0; i<4; i++){
			key[i] = data[4 + i];
			mask_key[i] = key[i];
		}

		for (auto i = 0; i<static_cast<int>(len_2); i++)
			actual_data[i] = data[8 + i];

		len = len_2;
	}
	else if (len == 127) {
		//TODO: handled for packet length > 127
	}
	return static_cast<int>(len);
}

void websocket_helper::un_mask_data(const unsigned char * key, const unsigned char * data, const int len, unsigned char * un_masked_data)
{
	auto j = 0;

	for (auto i = 0; i<len; i++) {
		j = i % 4;
		un_masked_data[i] = data[i] ^ key[j];
	}

	un_masked_data[len] = '\0';
}

void websocket_helper::websocket_framing(std::string m_tx_buffer, unsigned char * buffer_to_send)
{
	const auto fin = true; // is final frame?
	const auto rsv_1 = false; // Extension Related Must be Zero Unless Extended
	const auto rsv_2 = false; // Extension Related Must be Zero Unless Extended
	const auto rsv_3 = false; // Extension Related Must be Zero Unless Extended
	const unsigned char opcode = 0x01; //opcode leave it as 0x01, for text frame
	const auto mask = false; // Yes! unless client wants it masked
	unsigned int payload_len = 0;
	const auto m_tx_buffer_size = m_tx_buffer.size();
	auto len = 0;

	// Set Payload Length
	if (m_tx_buffer.size() <= 125)
		payload_len = m_tx_buffer.size();
	else if ((m_tx_buffer.size() <= 0x0000ffff) & (m_tx_buffer.size() >= 126))
		payload_len = 126;
	else
		payload_len = 127;

	//Do some calc...magic to put it in buffer
	buffer_to_send[0] = 0x00; // Do we need to do this? yes we do
	buffer_to_send[0] = buffer_to_send[0] | (fin << 7);
	buffer_to_send[0] = buffer_to_send[0] | (rsv_1 << 6);
	buffer_to_send[0] = buffer_to_send[0] | (rsv_2 << 5);
	buffer_to_send[0] = buffer_to_send[0] | (rsv_3 << 4);

	buffer_to_send[0] = buffer_to_send[0] | opcode;
	buffer_to_send[1] = 0x00;
	buffer_to_send[1] = buffer_to_send[1] | (mask << 7);
	buffer_to_send[1] = buffer_to_send[1] | (payload_len & 0xff);
	buffer_to_send[2] = 0x00; // Do we need to do this? yes we do

	// Two different buffer sizes allowed one less than 126 other more

	if (payload_len == 126){

		buffer_to_send[3] = m_tx_buffer_size & 0xff;
		buffer_to_send[2] = (m_tx_buffer_size & 0xff00) >> 8;
		
		for (auto i = 0; i<static_cast<int>(m_tx_buffer.size()); i++)
			buffer_to_send[i + 8] = m_tx_buffer[i];

		len = m_tx_buffer.size() + 4;
	}
	else {
		for (auto i = 0; i<static_cast<int>(m_tx_buffer.size()); i++)
			buffer_to_send[i + 2] = m_tx_buffer[i];

		len = m_tx_buffer.size() + 2;
	}
	buffer_to_send[len] = '\0';
}

unsigned char*  websocket_helper::create_handshake_message(unsigned char * data)
{
	auto protocol = get_header_value(data, "Sec-WebSocket-Version: ");

	if (atoi(reinterpret_cast<char *>(protocol)) >= 8)
		return set_challenge_response(data);

	return nullptr;
}

unsigned char* websocket_helper::set_challenge_response(unsigned char * data)
{
	const auto key = get_header_value(data, "Sec-WebSocket-Key: ");
	const auto protocol = get_header_value(data, "Sec-WebSocket-Protocol: ");

	const auto sss = std::string(reinterpret_cast<char*>(key))+"258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	auto* m_buf2 = string_to_char_array(sss);

	boost::uuids::detail::sha1 sha;
	sha.process_bytes(m_buf2, 60);
	unsigned int digest[5];
	sha.get_digest(digest);
	auto c = 0;
	
	unsigned char hash[20];
	for (auto i : digest)
	{
		hash[c++] = (i & 0xff000000) >> 24;
		hash[c++] = (i & 0xff0000) >> 16;
		hash[c++] = (i & 0xff00) >> 8;
		hash[c++] = i & 0xff;
	}
	
	encode64(m_buf2);

	std::stringstream ss;
	ss << "HTTP/1.1 101 Switching Protocols\r\n";
	ss << "Upgrade: websocket\r\n";
	ss << "Connection: Upgrade\r\n";
	ss << "Sec-WebSocket-Accept: " << m_buf2 <<"\r\n";
	ss << "Sec-WebSocket-Protocol: "<< protocol;
	ss << "\r\n\r\n";
	
	auto g = ss.str();

	return reinterpret_cast<unsigned char*>(&g[0]);
}

unsigned char* websocket_helper::get_header_value(unsigned char* data, const std::string& header)
{
	std::string value;
	std::string t(reinterpret_cast<char*>(data));
	const auto index = t.find(header);
	if (index != std::string::npos){
		value = t.substr(index + header.length());
		const auto end = value.find("\r\n");
		value = value.substr(0, end);
	}
	return string_to_char_array(value);
}

unsigned char * websocket_helper::string_to_char_array(const std::string& data)
{
	auto *a = new unsigned char[data.size() + 1];
	a[data.size()] = 0;
	memcpy(a, data.c_str(), data.size());
	return a;
}

std::string websocket_helper::encode64(unsigned char* val) {
	std::string t(reinterpret_cast<char*>(val));
    using namespace boost::archive::iterators;
    using it = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(it(std::begin(t)), it(std::end(t)));
    return tmp.append((3 - t.size() % 3) % 3, '=');
}