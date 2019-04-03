#pragma once
#include <string>
#include <boost/thread.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

class websocket_helper
{
public:
	websocket_helper();
	virtual ~websocket_helper();
	int parse_masked_data(unsigned char * key, const unsigned char * masked_data, unsigned char * un_masked_data);
	void websocket_framing(std::string m_tx_buffer, unsigned char* buffer_to_send);
	unsigned char* create_handshake_message(unsigned char* data);
	unsigned char* key;
private:
	unsigned char* set_challenge_response(unsigned char * data);
	unsigned char* get_header_value(unsigned char *data, const std::string& header);
	unsigned char* string_to_char_array(const std::string& data);
	std::string encode64(unsigned char* val);
};

